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


#ifndef ws2812Driver_h
#define ws2812Driver_h

#include <ESP8266WiFi.h>

#define PIX_MAX_BUFFER_SIZE 2040

#define PIX_LATCH_TIME 25       // 25 works for most

enum conf_type {
  WS2812_800KHZ,
  WS2812_400KHZ
};

class ws2812Driver {
  public:
    
    ws2812Driver(void);
    
    void setStrip(uint8_t port, uint8_t pin, uint16_t size, uint16_t config);
    void updateStrip(uint8_t port, uint16_t size, uint16_t config);
    
    uint8_t* getBuffer(uint8_t port);
    void clearBuffer(uint8_t port, uint16_t start);
    void clearBuffer(uint8_t port) {
      clearBuffer(port, 0);
    }
    void setBuffer(uint8_t port, uint16_t startChan, uint8_t* data, uint16_t size);
    
    byte setPixel(uint8_t port, uint16_t pixel, uint8_t r, uint8_t g, uint8_t b);
    byte setPixel(uint8_t port, uint16_t pixel, uint32_t colour);
    uint32_t getPixel(uint8_t port);
    
    bool show() __attribute__ ((optimize(0)));
    
    uint16_t numPixels(uint8_t port);
    
    byte buffer[2][PIX_MAX_BUFFER_SIZE];
    
    bool allowInterruptSingle = true;
    bool allowInterruptDouble = true;
    
    void doAPA106(byte* data, uint8_t pin, uint16_t numBytes);
    void doPixel(byte* data, uint8_t pin, uint16_t numBytes);
    
  private:
    void doPixelDouble(byte* data1, uint8_t pin1, byte* data2, uint8_t pin2, uint16_t numBytes);
    
    uint8_t _pin[2];
    uint16_t _pixels[2];
    uint16_t _config[2];
    uint32_t _nextPix = 0;
};

#endif
