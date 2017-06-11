/*
ESP8266_ArtNetNode v2.0.0
Copyright (c) 2016, Matthew Tong
https://github.com/mtongnz/ESP8266_ArtNetNode_v2

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.
If not, see http://www.gnu.org/licenses/
*/



// Timings from here:
// https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/

#include "ws2812Driver.h"


ws2812Driver::ws2812Driver() {
  _pixels[0] = 0;
  _pixels[1] = 0;  
}

void ws2812Driver::setStrip(uint8_t port, uint8_t pin, uint16_t size, uint16_t config) {
  _pin[port] = pin;
  _pixels[port] = size  * 3;
  _config[port] = config;

  pinMode(_pin[port], OUTPUT);
  digitalWrite(_pin[port], LOW);

  clearBuffer(port);
  
  // Clear the strip
  byte* b = buffer[port];
  //doPixel(b, _pin[port], PIX_MAX_BUFFER_SIZE);
}

void ws2812Driver::updateStrip(uint8_t port, uint16_t size, uint16_t config) {
  size = size * 3;
  
  // Clear the strip if it's shorter than our current strip
  if (size < _pixels[port] || _config[port] != config) {
    clearBuffer(port, size);

    // Wait for last pixel packet to finish it's latch time
    while (_nextPix > millis())
      yield();

    byte* b = buffer[port];
    //doPixel(b, _pin[port], _pixels[port]);
    
    // Allow at least 50 us with LOW to make LEDs latch data
    _nextPix = millis() + 5;
  }
  
  _pixels[port] = size;
  _config[port] = config;
}

uint8_t* ws2812Driver::getBuffer(uint8_t port) {
  return buffer[port];
}

void ws2812Driver::clearBuffer(uint8_t port, uint16_t start) {
  memset(&buffer[port][start], 0, PIX_MAX_BUFFER_SIZE - start);
}

void ws2812Driver::setBuffer(uint8_t port, uint16_t startChan, uint8_t* data, uint16_t size) {
  uint8_t* a = buffer[port];
  
  memcpy(&a[startChan], data, size);
}

byte ws2812Driver::setPixel(uint8_t port, uint16_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t* a = buffer[port];
  
  uint16_t chan = pixel * 3;

  // ws2812 is GRB ordering
  a[chan + 1] = r;
  a[chan] = g;
  a[chan + 2] = b;
}

byte ws2812Driver::setPixel(uint8_t port, uint16_t pixel, uint32_t colour) {
  setPixel(port, pixel, ((colour >> 16) & 0xFF), ((colour >> 8) & 0xFF), (colour & 0xFF));
}

uint32_t ws2812Driver::getPixel(uint8_t port) {
  uint8_t* b = buffer[port];
  uint16_t chan = _pixels[port] * 3;

  // ws2812 is GRB ordering - return RGB
  return ((b[chan + 1] << 16) | (b[chan] << 8) | (b[chan+2]));
}

uint16_t ws2812Driver::numPixels(uint8_t port) {
  return _pixels[port] / 3;
}


bool ws2812Driver::show() {
  if (_nextPix > millis())
    return 0;

  if (_pixels[0] == 0 && _pixels[1] == 0)
    return 1;
  
  yield();
  
  byte* b0 = buffer[0];
  byte* b1 = buffer[1];
  
  if (_pixels[0] == 0)
    doPixel(b1, _pin[1], _pixels[1]);
  else if (_pixels[1] == 0)
    doPixel(b0, _pin[0], _pixels[0]);
  else if (_pixels[1] > _pixels[0])
    doPixelDouble(b0, _pin[0], b1, _pin[1], _pixels[1]);
  else
    doPixelDouble(b0, _pin[0], b1, _pin[1], _pixels[0]);

  _nextPix = millis() + PIX_LATCH_TIME;
  
  return 1;
}

void ICACHE_RAM_ATTR ws2812Driver::doPixel(byte* data, uint8_t pin, uint16_t numBytes) {
  uint8_t a, b, c, d, f;
  uint32_t cc1, cc2;
  pin = (1 << pin);
  
  asm volatile (
    "MOVI %[r_set], 0x60000304;"
    
    "RSIL   %[r_int], 15;"                      // disable interrupts

    "doNextByteSingle:"
      "BEQZ   %[r_num_bytes], doExitSingle;"          // exit if all bytes sent
      
      "L8UI   %[r_data], %[r_data_array], 0;"         // Load array element
      "ADDI.N %[r_data_array], %[r_data_array], 1;"   // Move pointer to next array element
      "ADDI.N %[r_num_bytes], %[r_num_bytes], -1;"    // Decrement number of bytes left
      "MOVI %[r_bit], 0x80;"                          // Set our bitmask
    
    
    "sendNextBitSingle:"
      "BALL %[r_data], %[r_bit], doOne;"          // check if bit is one -> doOne
      "j doZero;"                                 // doZero if it's not


    "doNextBitSingle:"
      "SRLI %[r_bit], %[r_bit], 1;"               // shift bitmask right
      "BEQZ %[r_bit], doNextByteSingle;"          // get the next byte if all bits done
      "j sendNextBitSingle;"                      // send the next bit

    "SingleLoop:"
      "RSR %[r_cc2], CCOUNT;"                       // Get clock cycles
      "BGE %[r_cc1], %[r_cc2], SingleLoop;"         // If finishtime >= nowtime -> loop again
      "j doNextBitSingle;"                          // otherwise do the next bit

    
    "doOne:"
      "S16I  %[r_pin], %[r_set], 0;"  // set
      "MEMW;"
  
      "RSR %[r_cc1], CCOUNT;"           // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 95;"    // add to cycles for delay - works at 100

    "SingleOneLoop:"
      "RSR %[r_cc2], CCOUNT;"                         // Get clock cycles
      "BGE %[r_cc1], %[r_cc2], SingleOneLoop;"       // If finishtime >= nowtime -> loop again
      
      "S16I  %[r_pin], %[r_set], 4;"      // clear
      "MEMW;"
      
      "RSR %[r_cc1], CCOUNT;"             // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 61;"      // add to cycles for delay - works at 67
      "BEQZ %[r_allow_int], SingleLoop;"  // if allowInt equals false, jump without enabling interrupts
      "RSIL   %[r_int], 0;"               // enable interrupts again
      "NOP;"                              // 1 clock for any interrupts to run
      "RSIL   %[r_int], 15;"              // disable interrupts
      "j SingleLoop;"
  


    "doZero:"
      "S16I  %[r_pin], %[r_set], 0;"      // set
      "MEMW;"

      "RSR %[r_cc1], CCOUNT;"             // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 40;"      // add to cycles for delay - works at 50

    "SingleZeroLoop:"
      "RSR %[r_cc2], CCOUNT;"                         // Get clock cycles
      "BGE %[r_cc1], %[r_cc2], SingleZeroLoop;"       // If finishtime >= nowtime -> loop again

      "S16I  %[r_pin], %[r_set], 4;"      // clear
      "MEMW;"
  
      "RSR %[r_cc1], CCOUNT;"             // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 85;"      // add to cycles for delay   - works at 105
      "BEQZ %[r_allow_int], SingleLoop;"  // if allowInt equals false, jump without enabling interrupts
      "RSIL   %[r_int], 0;"               // enable interrupts again
      "NOP;"                              // 1 clock for any interrupts to run
      "RSIL   %[r_int], 15;"              // disable interrupts
      "j SingleLoop;"
  

    // Our exit point
    "doExitSingle:"
      "RSIL   %[r_int], 0;"   // enable interrupts again
    
    : [r_allow_int] "+r" (allowInterruptSingle), [r_int] "+r" (f), [r_cc1] "+r" (cc1), [r_cc2] "+r" (cc2), [r_set] "+r" (a), [r_bit] "+r" (b), [r_byte_count] "+r" (c), [r_data] "+r" (d), [r_pin] "+r" (pin), [r_data_array] "+r" (&data[0]), [r_num_bytes] "+r" (numBytes)
  );
}

void ICACHE_RAM_ATTR ws2812Driver::doPixelDouble(byte* data1, uint8_t pin1, byte* data2, uint8_t pin2, uint16_t numBytes) {
  uint8_t a, b, c, d, e, f;
  uint32_t cc1, cc2;
  pin1 = (1 << pin1);
  pin2 = (1 << pin2);
  
  asm volatile (
    "MOVI %[r_set], 0x60000304;"
    
    "RSIL   %[r_int], 15;"                        // disable interrupts

    "doNextByteDouble:"
      "BEQZ   %[r_num_bytes], doExitDouble;"            // exit if all bytes sent
      
      "L8UI   %[r_data1], %[r_data_array1], 0;"         // Load array elements
      "L8UI   %[r_data2], %[r_data_array2], 0;"
      
      "ADDI.N %[r_data_array1], %[r_data_array1], 1;"   // Move pointer to next array element
      "ADDI.N %[r_data_array2], %[r_data_array2], 1;"
      
      "ADDI.N %[r_num_bytes], %[r_num_bytes], -1;"      // Decrement number of bytes left
      "MOVI %[r_bit], 0x80;"                            // Set our bitmask
    
    
    "sendNextBitDouble:"
      "BALL %[r_data1], %[r_bit], doOne1Check2;"    // check if bit is one -> doOne
      "j doZero1Check2;"                            // doZero if it's not


    "doNextBitDouble:"
      "SRLI %[r_bit], %[r_bit], 1;"                 // shift bitmask right
      "BEQZ %[r_bit], doNextByteDouble;"            // get the next byte if all bits done
      "j sendNextBitDouble;"                        // send the next bit


    "doOne1Check2:"
      "BALL %[r_data2], %[r_bit], doOneOne;"        // Both bits are one -> doOneOne
      "j doOneZero;"                                // data1 is one, data2 is zero -> doOneZero


    "doZero1Check2:"
      "BALL %[r_data2], %[r_bit], doZeroOne;"       // data1 is zero, data2 is one -> doZeroOne
      "j doZeroZero;"                                // both bits are zero -> doZeroZero


    "DoubleLoop:"
      "RSR %[r_cc2], CCOUNT;"                       // Get clock cycles
      "BGE %[r_cc1], %[r_cc2], DoubleLoop;"         // If finishtime >= nowtime -> loop again
      "j doNextBitDouble;"                          // otherwise do the next bit

    
    "doOneOne:"
      "S16I  %[r_pin1], %[r_set], 0;"  // set
      "S16I  %[r_pin2], %[r_set], 0;"  // set
      "MEMW;"

      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop;"
      
      "S16I  %[r_pin1], %[r_set], 4;" // clear
      "S16I  %[r_pin2], %[r_set], 4;" // clear
      "MEMW;"
      
      "RSR %[r_cc1], CCOUNT;"             // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 65;"      // add to cycles for delay
      "BEQZ %[r_allow_int], DoubleLoop;"  // if allowInt equals false, jump without enabling interrupts
      "RSIL   %[r_int], 0;"               // enable interrupts again
      "NOP;"                              // 1 clock for any interrupts to run
      "RSIL   %[r_int], 15;"              // disable interrupts
      "j DoubleLoop;"
      

    "doZeroZero:"
      "S16I  %[r_pin1], %[r_set], 0;"  // set
      "S16I  %[r_pin2], %[r_set], 0;"  // set
      "MEMW;"
  
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      
      "S16I  %[r_pin1], %[r_set], 4;"  // clear
      "S16I  %[r_pin2], %[r_set], 4;"  // clear
      "MEMW;"

      "RSR %[r_cc1], CCOUNT;"             // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 105;"     // add to cycles for delay
      "BEQZ %[r_allow_int], DoubleLoop;"  // if allowInt equals false, jump without enabling interrupts
      "RSIL   %[r_int], 0;"               // enable interrupts again
      "NOP;"                              // 1 clock for any interrupts to run
      "RSIL   %[r_int], 15;"              // disable interrupts
      "j DoubleLoop;"
      

    "doOneZero:"
      "S16I  %[r_pin1], %[r_set], 0;"  // set
      "S16I  %[r_pin2], %[r_set], 0;"  // set
      "MEMW;"
  
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
  
      "S16I  %[r_pin2], %[r_set], 4;" // clear
      
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      
      "S16I  %[r_pin1], %[r_set], 4;" // clear
      "MEMW;"

      "RSR %[r_cc1], CCOUNT;"             // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 65;"      // add to cycles for delay
      "BEQZ %[r_allow_int], DoubleLoop;"  // if allowInt equals false, jump without enabling interrupts
      "RSIL   %[r_int], 0;"               // enable interrupts again
      "NOP;"                              // 1 clock for any interrupts to run
      "RSIL   %[r_int], 15;"              // disable interrupts
      "j DoubleLoop;"
      

    "doZeroOne:"
      "S16I  %[r_pin1], %[r_set], 0;"  // set
      "S16I  %[r_pin2], %[r_set], 0;"  // set
      "MEMW;"
  
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
  
      "S16I  %[r_pin1], %[r_set], 4;" // clear
      
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      "_nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; _nop; "
      
      "S16I  %[r_pin2], %[r_set], 4;" // clear
      "MEMW;"

      "RSR %[r_cc1], CCOUNT;"             // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 65;"      // add to cycles for delay
      "BEQZ %[r_allow_int], DoubleLoop;"  // if allowInt equals false, jump without enabling interrupts
      "RSIL   %[r_int], 0;"               // enable interrupts again
      "NOP;"                              // 1 clock for any interrupts to run
      "RSIL   %[r_int], 15;"              // disable interrupts
      "j DoubleLoop;"
      


    // Our exit point
    "doExitDouble:"
      "RSIL   %[r_int], 15;"                        // disable interrupts
      
    : [r_allow_int] "+r" (allowInterruptDouble), [r_int] "+r" (f), [r_cc1] "+r" (cc1), [r_cc2] "+r" (cc2), [r_set] "+r" (a), [r_bit] "+r" (b), [r_byte_count] "+r" (c), [r_data1] "+r" (d), [r_pin1] "+r" (pin1), [r_data_array1] "+r" (&data1[0]), [r_data2] "+r" (e), [r_pin2] "+r" (pin2), [r_data_array2] "+r" (&data2[0]), [r_num_bytes] "+r" (numBytes)
  );
}

void ICACHE_RAM_ATTR ws2812Driver::doAPA106(byte* data, uint8_t pin, uint16_t numBytes) {
  uint8_t a, b, c, d, f;
  uint32_t cc1, cc2, cc3;
  pin = (1 << pin);
  
  asm volatile (
    "MOVI %[r_set], 0x60000304;"
    
    "RSIL   %[r_int], 15;"                      // disable interrupts

    "doNextByte106:"
      "BEQZ   %[r_num_bytes], doExit106;"             // exit if all bytes sent
      
      "L8UI   %[r_data], %[r_data_array], 0;"         // Load array element
      "ADDI.N %[r_data_array], %[r_data_array], 1;"   // Move pointer to next array element
      "ADDI.N %[r_num_bytes], %[r_num_bytes], -1;"    // Decrement number of bytes left
      "MOVI %[r_bit], 0x80;"                          // Set our bitmask
    
    
    "sendNextBit106:"
      "RSR %[r_cc3], CCOUNT;"                     // Get clock cycles
      "ADDI %[r_cc3], %[r_cc3], 230;"             // add to cycles - total bit length
      "BALL %[r_data], %[r_bit], doOne106;"       // check if bit is one -> doOne
      "j doZero106;"                              // doZero if it's not


    "doNextBit106:"
      "SRLI %[r_bit], %[r_bit], 1;"               // shift bitmask right
      "BEQZ %[r_bit], doNextByte106;"             // get the next byte if all bits done
      "j sendNextBit106;"                         // send the next bit

    "Loop106:"
      "RSR %[r_cc2], CCOUNT;"                       // Get clock cycles
      "BGE %[r_cc3], %[r_cc2], Loop106;"            // If finishtime >= nowtime -> loop again
      "j doNextBit106;"                             // otherwise do the next bit

    
    "doOne106:"
      "S16I  %[r_pin], %[r_set], 0;"                  // set pin
      "MEMW;"
  
      "RSR %[r_cc1], CCOUNT;"                         // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 185;"                 // add to cycles for delay

    "OneLoop106:"
      "RSR %[r_cc2], CCOUNT;"                         // Get clock cycles
      "BGE %[r_cc1], %[r_cc2], OneLoop106;"           // If finishtime >= nowtime -> loop again
      
      "S16I  %[r_pin], %[r_set], 4;"                  // clear pin
      "MEMW;"
      
      "j Loop106;"
  


    "doZero106:"
      "S16I  %[r_pin], %[r_set], 0;"                  // set pin
      "MEMW;"

      "RSR %[r_cc1], CCOUNT;"                         // get clock cycles
      "ADDI %[r_cc1], %[r_cc1], 40;"                  // add to cycles for delay

    "ZeroLoop106:"
      "RSR %[r_cc2], CCOUNT;"                         // Get clock cycles
      "BGE %[r_cc1], %[r_cc2], ZeroLoop106;"          // If finishtime >= nowtime -> loop again

      "S16I  %[r_pin], %[r_set], 4;"                  // clear pin
      "MEMW;"
  
      "j Loop106;"
  

    // Our exit point
    "doExit106:"
      "RSIL   %[r_int], 0;"   // enable interrupts again
    
    : [r_int] "+r" (f), [r_cc1] "+r" (cc1), [r_cc2] "+r" (cc2), [r_cc3] "+r" (cc3), [r_set] "+r" (a), [r_bit] "+r" (b), [r_byte_count] "+r" (c), [r_data] "+r" (d), [r_pin] "+r" (pin), [r_data_array] "+r" (&data[0]), [r_num_bytes] "+r" (numBytes)
  );
}
