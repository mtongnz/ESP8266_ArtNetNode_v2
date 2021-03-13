/*
espDMX v2 library
Copyright (c) 2016, Matthew Tong
https://github.com/mtongnz/espDMX

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see http://www.gnu.org/licenses/
*/

#include "espDMX_RDM.h"

espDMX dmxA(0);
espDMX dmxB(1);

void dmx_interrupt_handler(void);

uint16_t dmx_get_tx_fifo_room(dmx_t* dmx);
void dmx_interrupt_enable(dmx_t* dmx);
void dmx_interrupt_arm(dmx_t* dmx);
void dmx_interrupt_disarm(dmx_t* dmx);
void rdm_interrupt_arm(dmx_t* dmx);
void rdm_interrupt_disarm();
void dmx_set_baudrate(dmx_t* dmx, int baud_rate);
void dmx_set_chans(dmx_t* dmx, uint8_t* data, uint16_t numChans, uint16_t startChan);
void dmx_buffer_update(dmx_t* dmx, uint16_t num);
int dmx_state(dmx_t* dmx);
void rx_flush();
void dmx_flush(dmx_t* dmx);
static void uart_ignore_char(char c);

void dmx_set_buffer(dmx_t* dmx, byte* buf);

void dmx_uninit(dmx_t* dmx);

static bool timer1Set = false;
static bool rdmInUse = false;
static bool rdmBreak = false;
static bool rdm_pause = false;
static bool dmx_input = false;
static uint8_t rxUser;
static unsigned long rdmTimer = 0;


void ICACHE_RAM_ATTR dmx_interrupt_handler(void) {
  // stop other interrupts for TX
  noInterrupts();
  
  if(U0IS & (1 << UIFE)) {    // TX0 Fifo Empty
    U0IC = (1 << UIFE);       // clear status flag
    dmxA._transmit();
  }

  if(U1IS & (1 << UIFE)) {    // TX1 Fifo Empty
    U1IC = (1 << UIFE);       // clear status flag
    dmxB._transmit();
  }

  interrupts();


  // RDM replies
  if (rdmInUse) {
    if ((U0IS & (1 << UIBD)) || ( U0IS & (1 << UIFR))) {    // RX0 Break Detect
      U0IC = (1 << UIBD) | (1 << UIFR);                     // clear status flags
      rdmBreak = true;
    }

    if(U0IS & (1 << UIFF)) {    // RX0 Fifo Full
      if (rxUser == 0)
        dmxA.rdmReceived();
      else
        dmxB.rdmReceived();
    }

  // DMX input
  } else if (dmx_input) {
    
    // Data received
    while (U0IS & (1 << UIFF)) {
      if (rxUser == 0)
        dmxA.dmxReceived((uint8_t)USF(0));
      else
        dmxB.dmxReceived((uint8_t)USF(0));
      
      U0IC = (1 << UIFF);   // Clear interrupt
    }
  
    // Break/Frame error detect
    if ((U0IS & (1 << UIBD)) || ( U0IS & (1 << UIFR))) {    // RX0 Break Detect
      U0IC = (1 << UIBD) | (1 << UIFR);                     // clear status flags

      if (rxUser == 0)
        dmxA.inputBreak();
      else
        dmxB.inputBreak();
    }
  }
}

static void uart_ignore_char(char c) { return; }

uint16_t dmx_get_tx_fifo_room(dmx_t* dmx) {
    if(dmx == 0 || dmx->state == DMX_NOT_INIT)
        return 0;
    return UART_TX_FIFO_SIZE - ((USS(dmx->dmx_nr) >> USTXC) & 0xff);
}

void dmx_flush(dmx_t* dmx) {
    if(dmx == 0 || dmx->state == DMX_NOT_INIT)
        return;

    uint32_t tmp = 0x00000000;
    tmp |= (1 << UCTXRST);

    // Clear TX Fifo
    USC0(dmx->dmx_nr) |= (tmp);
    USC0(dmx->dmx_nr) &= ~(tmp);
}

void rx_flush() {
    uint32_t tmp = 0x00000000;
    tmp |= (1 << UCRXRST);

    // Clear RX Fifo
    USC0(0) |= (tmp);
    USC0(0) &= ~(tmp);
}

void dmx_interrupt_enable(dmx_t* dmx) {
    if(dmx == 0 || dmx->state == DMX_NOT_INIT)
        return;

    // Clear all interupt bits
    USIC(dmx->dmx_nr) = 0x1ff;

    if (dmx->dmx_nr == 1) {
      // Set TX Fifo Empty trigger point
      USC1(1) = (0 << UCFET);

      // UART at 250k for DMX data
      USD(1) = (ESP8266_CLOCK / DMX_TX_BAUD);
      USC0(1) = DMX_TX_CONF;
    }
    
    // Attach out interupt handler function
    ETS_UART_INTR_ATTACH(&dmx_interrupt_handler, NULL);

    // Enable UART Interrupts
    ETS_UART_INTR_ENABLE();

    // UART0 setup
    if (!timer1Set) {
      timer1Set = true;

      // UART at 250k for DMX data
      USD(0) = (ESP8266_CLOCK / DMX_TX_BAUD);
      USC0(0) = DMX_TX_CONF;
      USC1(0) = (127 << UCFFT);

      // Disable RX Fifo Full & Break Detect & Frame Error Interupts
      //USIE(0) &= ~((1 << UIFF) | (1 << UIBD) | (1 << UIFR));
    }
}

void dmx_interrupt_arm(dmx_t* dmx) {
  if(dmx == 0 || dmx->state == DMX_NOT_INIT)
      return;

  // Clear all interupt bits
  USIC(dmx->dmx_nr) = 0xffff;
  
  // Enable TX Fifo Empty Interupt
  USIE(dmx->dmx_nr) |= (1 << UIFE);
}

void dmx_interrupt_disarm(dmx_t* dmx) {
  if(dmx == 0 || dmx->state == DMX_NOT_INIT)
      return;

  USIE(dmx->dmx_nr) &= ~(1 << UIFE);
}

void rdm_interrupt_arm(dmx_t* dmx) {
  if(dmx == 0 || dmx->state == DMX_NOT_INIT)
      return;
  
  // Enable RX Fifo Full & Break Detect & Frame Error Interupts
  USIE(0) |= (1 << UIFF) | (1 << UIBD) | (1 << UIFR);

  digitalWrite(dmx->dirPin, LOW);
  rdmBreak = false;
  rxUser = dmx->dmx_nr;
  dmx->rx_pos = 0;
  dmx->rdm_response.clear();

  // Timer1 start
  // T1L = ((RDM_LISTEN_TIME)& 0x7FFFFF);
  // TEIE |= TEIE1;//edge int enable

  rdmTimer = micros() + 3000;
}

void rdm_interrupt_disarm() {
  // Disable RX Fifo Full & Break Detect & Frame Error Interupts
  USIE(0) &= ~((1 << UIFF) | (1 << UIBD) | (1 << UIFR));

  // TEIE &= ~TEIE1;//edge int disable
  // T1L = 0;

  rdmInUse = false;
}

void dmx_set_baudrate(dmx_t* dmx, int baud_rate) {
  if(dmx == 0 || dmx->state == DMX_NOT_INIT)
      return;

  USD(dmx->dmx_nr) = (ESP8266_CLOCK / baud_rate);
}

void dmx_clear_buffer(dmx_t* dmx) {
  for (int i = 0; i < 512; i++)
    dmx->data[i] = 0;
  
  dmx->numChans = DMX_MIN_CHANS;
}

void dmx_set_buffer(dmx_t* dmx, byte* buf) {
  if(dmx == 0 || dmx->state == DMX_NOT_INIT)
    return;

  if (dmx->ownBuffer)
    os_free(dmx->data);

  if (buf == NULL) {
    buf = (byte*) os_malloc(sizeof(byte) * 512);

    if(!buf) {
      os_free(buf);
      dmx->ownBuffer = 0;
      return;
    }
    dmx->ownBuffer = 1;
  } else
    dmx->ownBuffer = 0;

  dmx->data = buf;

}

void dmx_uninit(dmx_t* dmx) {
    if(dmx == 0 || dmx->state == DMX_NOT_INIT)
        return;

    dmx_interrupt_disarm(dmx);
    dmx_flush(dmx);

    pinMode(dmx->txPin, OUTPUT);
    digitalWrite(dmx->txPin, HIGH);

    // Set DMX direction to input so no garbage is sent out
    if (dmx->dirPin != 255)
      digitalWrite(dmx->dirPin, LOW);

    if (dmx->dmx_nr == rxUser) {
      rdm_interrupt_disarm();
      rx_flush();
    }

    if (dmx->rdm_enable) {
      dmx->rdm_enable = 0;
      digitalWrite(dmx->dirPin, HIGH);

      dmx->todManID = (uint16_t*)realloc(dmx->todManID, 0);
      dmx->todDevID = (uint32_t*)realloc(dmx->todDevID, 0);

      dmx->rdmCallBack = NULL;
      dmx->todCallBack = NULL;
    }

    os_free(dmx->data1);
    dmx->data1 = 0;

    dmx->isInput = false;
    dmx->inputCallBack = NULL;
  
    if (dmx->ownBuffer)
      os_free(dmx->data);
}

int dmx_get_state(dmx_t* dmx) {
  return dmx->state;
}

void dmx_set_state(dmx_t* dmx, int state) {
  dmx->state = state;
}

void dmx_set_chans(dmx_t* dmx, uint8_t* data, uint16_t num, uint16_t start) {
  if(dmx == 0 || dmx->state == DMX_NOT_INIT)
    return;

  dmx->started = true;

  uint16_t newNum = start + num - 1;
  if (newNum > 512)
    newNum = 512;

  // Is there any new channel data
  if (memcmp(data, &(dmx->data[start-1]), num) != 0) {
    // Find the highest channel with new data
    for (; newNum >= dmx->numChans; newNum--, num--) {
      if (dmx->data[newNum-1] != data[num-1])
        break;
    }
    newNum += DMX_ADD_CHANS;

    // If we receive tiny data input, just output minimum channels
    if (newNum < DMX_MIN_CHANS)
      newNum = DMX_MIN_CHANS;
      
    // Put data into our buffer
    memcpy(&(dmx->data[start-1]), data, num);

    if (newNum > dmx->numChans)
      dmx->numChans = (newNum > 512) ? 512 : newNum;
    dmx->newDMX = true;
    //dmx_transmit(dmx);
  }
}

void dmx_buffer_update(dmx_t* dmx, uint16_t num) {
  if(dmx == 0 || dmx->state == DMX_NOT_INIT || num <= dmx->numChans)
    return;

  dmx->started = true;

  if (num > 512)
    num = 512;

  // Find the highest channel with data
  for (; num >= dmx->numChans; num--) {
    if (dmx->data[num-1] != 0)
      break;
  }
  num += DMX_ADD_CHANS;

  // If we receive tiny data input, just output minimum channels
  if (num < DMX_MIN_CHANS)
    num = DMX_MIN_CHANS;
      
  if (num > dmx->numChans)
    dmx->numChans = (num > 512) ? 512 : num;

  dmx->newDMX = true;
  //dmx_transmit(dmx);
}

espDMX::espDMX(uint8_t dmx_nr) :
  _dmx_nr(dmx_nr), _dmx(0) {
}

espDMX::~espDMX(void) {
  end();
}

void espDMX::begin(uint8_t dir, byte* buf) {
  if(_dmx == 0) {
    _dmx = (dmx_t*) os_malloc(sizeof(dmx_t));
    
    if(_dmx == 0) {
      os_free(_dmx);
      _dmx = 0;
      return;
    }

    _dmx->data1 = (byte*) os_malloc(sizeof(byte) * 512);
    memset(_dmx->data1, 0, 512);

    _dmx->ownBuffer = 0;

    system_set_os_print(0);
    ets_install_putc1(&uart_ignore_char);
    
    // Initialize variables
    _dmx->dmx_nr = _dmx_nr;
    _dmx->txPin = (_dmx->dmx_nr == 0) ? 1 : 2;
    _dmx->state = DMX_STOP;
    _dmx->txChan = 0;
    _dmx->full_uni_time = 0;
    _dmx->last_dmx_time = 0;
    _dmx->led_timer = 0;
    _dmx->newDMX = false;
    _dmx->started = false;
    _dmx->rdm_enable = false;
    _dmx->dirPin = dir;		// 255 is used to indicate no dir pin

    _dmx->rdmCallBack = NULL;
    _dmx->todCallBack = NULL;
    _dmx->isInput = false;
    _dmx->inputCallBack = NULL;


    // TX output set to idle
    pinMode(_dmx->txPin, OUTPUT);
    digitalWrite(_dmx->txPin, HIGH);

    // Set direction to output
    if (_dmx->dirPin != 255) {
      pinMode(_dmx->dirPin, OUTPUT);
      digitalWrite(_dmx->dirPin, HIGH);
    }
  }

  if (_dmx) {
    dmx_set_buffer(_dmx, buf);
    dmx_clear_buffer(_dmx);
    dmx_interrupt_enable(_dmx);
  }
}

void espDMX::setBuffer(byte* buf) {
  dmx_set_buffer(_dmx, buf);
}

void espDMX::pause() {
  dmx_interrupt_disarm(_dmx);

  dmx_flush(_dmx);

  digitalWrite(_dmx->dirPin, HIGH);
}

void espDMX::unPause() {
  if(_dmx == 0 || _dmx->state == DMX_NOT_INIT)
    return;

  _dmx->newDMX = true;
  _dmx->state = DMX_STOP;

  digitalWrite(_dmx->dirPin, HIGH);

  //dmx_transmit(_dmx);
}

void espDMX::end() {
  if (_dmx == 0)
    return;

  dmx_uninit(_dmx);

  os_free(_dmx);

  _dmx = 0;
}

void espDMX::setChans(byte *data, uint16_t numChans, uint16_t startChan) {
  dmx_set_chans(_dmx, data, numChans, startChan);
}

void espDMX::chanUpdate(uint16_t numChans) {
  dmx_buffer_update(_dmx, numChans);
}

void espDMX::clearChans() {
  if(_dmx == 0 || _dmx->state == DMX_NOT_INIT)
    return;

  dmx_clear_buffer(_dmx);
}

byte *espDMX::getChans() {
  if(_dmx == 0 || _dmx->state == DMX_NOT_INIT)
    return 0;

  return _dmx->data;
}

uint16_t espDMX::numChans() {
  if(_dmx == 0 || _dmx->state == DMX_NOT_INIT)
    return 0;

  return _dmx->numChans;
}

void espDMX::ledIntensity(uint8_t newIntensity) {
  if(_dmx == 0 || _dmx->state == DMX_NOT_INIT)
    return;

  _dmx->ledIntensity = newIntensity;
}

void ICACHE_RAM_ATTR espDMX::_transmit(void) {
  
  // If we have data to transmit
  if (_dmx->txChan < _dmx->txSize) {
    
    // Keep the number of bytes sent low to keep it quick
//    uint16_t txSize = dmx->txSize - dmx->txChan;
//    txSize = (txSize > DMX_MAX_BYTES_PER_INT) ? DMX_MAX_BYTES_PER_INT : txSize;      

//    for(; txSize; --txSize)
      USF(_dmx->dmx_nr) = _dmx->data1[_dmx->txChan++];

//    dmx_interrupt_arm(dmx);

  // If all bytes are transmitted
  } else {

    //dmx_interrupt_disarm(_dmx);
    USIE(_dmx->dmx_nr) &= ~(1 << UIFE);

    if (_dmx->state == DMX_TX) {

      _dmx->state = DMX_STOP;

    } else if (!rdm_pause) { // if (_dmx->state == RDM_TX) {
      
      _dmx->state = RDM_RX;
      rdm_interrupt_arm(_dmx);
    }
  }
}

bool espDMX::rdmSendCommand(rdm_data* data) {
  if (_dmx == 0 || !_dmx->rdm_enable || _dmx->rdm_queue.isFull())
    return false;

  if (system_get_free_heap_size() < 2000)
    return false;

  byte packetLength = data->packet.Length;
  uint16_t checkSum = 0x0000;
  for (byte x = 0; x < packetLength; x++) {
    checkSum += data->buffer[x];
  }
  checkSum = checkSum % 0x10000;

  data->buffer[packetLength] = checkSum >> 8;
  data->buffer[packetLength + 1] = checkSum & 0xFF;

  bool r = _dmx->rdm_queue.push(data);
  
  return r;
}
        
bool espDMX::rdmSendCommand(uint8_t cmdClass, uint16_t pid, uint16_t manID, uint32_t devID, byte* data, uint16_t dataLength, uint16_t subDev) {
  if (_dmx == 0 || !_dmx->rdm_enable)
    return false;

  rdm_data command;

  // Note that all ints are stored little endian so we need to flip them
  // to get correct byte order
  
  command.packet.StartCode = (E120_SC_RDM << 8) | E120_SC_SUB_MESSAGE;
  command.packet.Length = 24 + dataLength;
  command.packet.DestMan = manID;
  command.packet.DestDev = devID;
  command.packet.SourceMan = _dmx->rdm_source_man;
  command.packet.SourceDev = _dmx->rdm_source_dev;
  command.packet.TransNo = _dmx->rdm_trans_no++;
  command.packet.ResponseType = 0x01;
  command.packet.MsgCount = 0;
  command.packet.SubDev = subDev;
  command.packet.CmdClass = cmdClass;
  command.packet.PID = pid;
  command.packet.DataLength = dataLength;
  if (dataLength > 0)
    memcpy(command.packet.Data, data, dataLength);

  return rdmSendCommand(&command);
}

void espDMX::rdmReceived() {
  if (_dmx == 0 || _dmx->state != RDM_RX)
    return;

  while((USS(0) >> USRXC) & 0x7F) {  
    _dmx->rdm_response.buffer[_dmx->rx_pos] = USF(0);

    // Handle multiple 0xFE to start discovery response
    if (_dmx->rx_pos == 1 && _dmx->rdm_response.buffer[0] == 0xFE && _dmx->rdm_response.buffer[1] == 0xFE)
      continue;

    // Handle break & MAB
    if (rdmBreak || _dmx->rdm_response.buffer[0] == 0) {
      _dmx->rx_pos = 0;
      rdmBreak = false;
      continue;
    }
    _dmx->rx_pos++;

    // Get packet size and adjust timer accordingly
//    unsigned long newTime = micros() + (44 * _dmx->rdm_response.buffer[2]);
//    if (newTime > rdmTimer)
//      rdmTimer = newTime;

//    if (_dmx->rx_pos == 3 && _dmx->rdm_response.buffer[0] == 0xCC && _dmx->rdm_response.buffer[1] == 0x01) {
//      uint16_t newTime = (_dmx->rdm_response.buffer[2] * RDM_BYTE_TIME);
//      if (newTime > T1L)
//        T1L = (newTime & 0x7FFFFF);
//    }
  }

  // Clear interupt flags
  USIC(0) = USIS(0);
}

void espDMX::rdmDiscovery(uint8_t discType) {
  if (!_dmx || !_dmx->rdm_enable)
    return;
  
  if (discType == RDM_DISCOVERY_TOD_WIPE) {
    _dmx->tod_size = 0;
      
    _dmx->todManID = (uint16_t*)realloc(_dmx->todManID, 0);
    _dmx->todDevID = (uint32_t*)realloc(_dmx->todDevID, 0);

    _dmx->tod_status = RDM_TOD_NOT_READY;
    
    discType = RDM_DISCOVERY_FULL;
  }

  byte startEnd[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
  if (discType == RDM_DISCOVERY_FULL) {
    _dmx->tod_changed = true;
    rdmSendCommand(E120_DISCOVERY_COMMAND, E120_DISC_UN_MUTE, 0xFFFF, 0xFFFFFFFF);
    rdmSendCommand(E120_DISCOVERY_COMMAND, E120_DISC_UNIQUE_BRANCH, 0xFFFF, 0xFFFFFFFF, startEnd, 12);

  // discType == RDM_DISCOVERY_INCREMENTAL
  } else {
    if (_dmx->rdm_discovery_pos >= _dmx->tod_size) {
      rdmSendCommand(E120_DISCOVERY_COMMAND, E120_DISC_UNIQUE_BRANCH, 0xFFFF, 0xFFFFFFFF, startEnd, 12);
    } else {
      rdmSendCommand(E120_DISCOVERY_COMMAND, E120_DISC_MUTE, _dmx->todManID[_dmx->rdm_discovery_pos], _dmx->todDevID[_dmx->rdm_discovery_pos]);
      _dmx->rdm_discovery_pos++;
    }
  }
}

void espDMX::rdmDiscoveryResponse(rdm_data* c) {
  // If we received nothing, branch is empty
  if (_dmx->rx_pos == 0) {
    byte a[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    // If it's a reply to the top branch, all devices are found
    if (memcmp(c->packet.Data, a, 12) == 0) {

      _dmx->rdm_last_discovery = millis();
      _dmx->rdm_discovery_pos = 0;

      _dmx->tod_status = RDM_TOD_READY;

      if (_dmx->tod_changed && _dmx->todCallBack != 0) {
        _dmx->tod_changed = false;
        _dmx->todCallBack();
      }

      // Issue un-mute to all so no devices hide on the next incremental discovery
      rdmSendCommand(E120_DISCOVERY_COMMAND, E120_DISC_UN_MUTE, 0xFFFF, 0xFFFFFFFF);
    }
    
    return;
  }
  
  // Check for correct length & no frame errors
  if (_dmx->rdm_response.discovery.headerFE == 0xFE && _dmx->rdm_response.discovery.headerAA == 0xAA) {
    uint16_t _manID;
    uint32_t _devID;
    byte* maskedDevID = _dmx->rdm_response.discovery.maskedDevID;
    byte* maskedChkSm = _dmx->rdm_response.discovery.maskedChecksum;

    _manID = (maskedDevID[0] & maskedDevID[1]);
    _manID = (_manID << 8)  + (maskedDevID[2] & maskedDevID[3]);
    _devID = (maskedDevID[4] & maskedDevID[5]);
    _devID = (_devID << 8)  + (maskedDevID[6] & maskedDevID[7]);
    _devID = (_devID << 8)  + (maskedDevID[8] & maskedDevID[9]);
    _devID = (_devID << 8)  + (maskedDevID[10] & maskedDevID[11]);

    // Calculate checksum
    uint16_t checkSum = 0;
    for (uint8_t x = 0; x < 12; x++)
      checkSum += maskedDevID[x];
    checkSum = checkSum % 10000;
    uint16_t mChk = (maskedChkSm[0] & maskedChkSm[1]);
    mChk = (mChk << 8) | (maskedChkSm[2] & maskedChkSm[3]);

    // If the checksum is valid
    if (checkSum == mChk) {
      // Send mute command to check device is there & to mute from further discovery requests
      rdmSendCommand(E120_DISCOVERY_COMMAND, E120_DISC_MUTE, _manID, _devID);
      
      // Recheck the branch
      c->packet.TransNo = _dmx->rdm_trans_no++;
      rdmSendCommand(c);
      
      return;
    }
  }

  // If we didn't get a valid response, split branch and try again

  uint64_t m = 0;
  uint64_t n = 0;
  uint64_t e = 0;

  // Get current end address
  for (uint8_t x = 6; x < 12; x++)
    e = (e << 8) | c->packet.Data[x];
  
  // Calculate the midpoint & midpoint + 1
  e = e << 16;
  m = e >> 1;
  n = m + 1;

  // Check if we're at the bottom branch
  if (n == e) {
    uint16_t a = __builtin_bswap16(e >> 32);
    uint32_t b = __builtin_bswap32(e & 0xFFFFFFFF);
    
    // Send mute command to check device is there & to mute from further discovery requests
    rdmSendCommand(E120_DISCOVERY_COMMAND, E120_DISC_MUTE, a, b);
    
    return;
  }

  // Bitswap to fix endianess
  m = __builtin_bswap64(m);
  e = __builtin_bswap64(e);
  n = __builtin_bswap64(n);
  
  // If we reach max queue size, wait for a bit and try again
  while (_dmx->rdm_queue.space() < 2) {
    yield();
  }

  // Send command for lower half
  memcpy(&c->packet.Data[6], &m, 6);
  c->packet.TransNo = _dmx->rdm_trans_no++;
  rdmSendCommand(c);
  
  // Send command for upper half
  memcpy(c->packet.Data, &n, 6);
  memcpy(&c->packet.Data[6], &e, 6);
  c->packet.TransNo = _dmx->rdm_trans_no++;
  rdmSendCommand(c);
}

void espDMX::rdmMuteResponse(rdm_data* c) {
  _dmx->rdm_response.endianFlip();
  
  // Check for correct length & ACK response
  if (_dmx->rx_pos > 15) {
    if (c->packet.DestMan == _dmx->rdm_response.packet.SourceMan && c->packet.DestDev == _dmx->rdm_response.packet.SourceDev && _dmx->rdm_response.packet.ResponseType == E120_RESPONSE_TYPE_ACK) {
      uint16_t checkSum = 0;
      uint8_t x = 0;
      
      for (; x < _dmx->rdm_response.packet.Length; x++)
        checkSum += _dmx->rdm_response.buffer[x];

      checkSum = checkSum % 10000;

      // Check the checksum
      if (_dmx->rdm_response.buffer[x] == (checkSum >> 8) && _dmx->rdm_response.buffer[x+1] == (checkSum & 0xFF)) {
        
        // Is the device already in our UID list
        for (uint16_t x = 0; x < _dmx->tod_size; x++) {
          if (_dmx->todManID[x] == _dmx->rdm_response.packet.SourceMan && _dmx->todDevID[x] == _dmx->rdm_response.packet.SourceDev) {
            
            if (x == _dmx->rdm_discovery_pos)
              _dmx->rdm_discovery_pos++;

            return;
          }
        }

        // Add the deivce to our UID list
        _dmx->todManID = (uint16_t*)realloc(_dmx->todManID, (_dmx->tod_size+1) * sizeof(uint16_t));
        _dmx->todDevID = (uint32_t*)realloc(_dmx->todDevID, (_dmx->tod_size+1) * sizeof(uint32_t));

        _dmx->todManID[_dmx->tod_size] = _dmx->rdm_response.packet.SourceMan;
        _dmx->todDevID[_dmx->tod_size] = _dmx->rdm_response.packet.SourceDev;

        _dmx->tod_size++;
        _dmx->tod_changed = true;

      }
    }


  // No response received
  
  } else {
    // Delete devices from TOD if they didn't respond
    for (uint16_t x = 0; x < _dmx->tod_size; x++) {
      if (_dmx->todManID[x] == c->packet.DestMan && _dmx->todDevID[x] == c->packet.DestDev) {

        // Shift all our devices up the list
        for (uint16_t y = x+1; y < _dmx->tod_size; y++) {
          _dmx->todManID[y-1] = _dmx->todManID[y];
          _dmx->todDevID[y-1] = _dmx->todDevID[y];
        }

        _dmx->tod_size--;
        _dmx->tod_changed = true;
      
        _dmx->rdm_discovery_pos = 0;

        _dmx->todManID = (uint16_t*)realloc(_dmx->todManID, _dmx->tod_size * sizeof(uint16_t));
        _dmx->todDevID = (uint32_t*)realloc(_dmx->todDevID, _dmx->tod_size * sizeof(uint32_t));
      
        return;
      }
    }
  }
}

void espDMX::rdmRXTimeout() {
  if (_dmx == 0)
    return;

  if (rdm_pause) {
    rdm_interrupt_disarm();
    dmx_flush(_dmx);

    _dmx->state = DMX_STOP;
    digitalWrite(_dmx->dirPin, HIGH);

    //dmx_transmit(_dmx);
    return;
  }

  // Get remaining data
  rdmReceived();
  
  _dmx->state = DMX_STOP;
  digitalWrite(_dmx->dirPin, HIGH);

  rdm_interrupt_disarm();

  rdm_data c;
  _dmx->rdm_queue.pop(&c);

  //dmx_transmit(_dmx);

  if (c.packet.CmdClass == E120_DISCOVERY_COMMAND) {
    if (c.packet.PID == E120_DISC_UNIQUE_BRANCH) {
      rdmDiscoveryResponse(&c);
      return;
    } else if (c.packet.PID == E120_DISC_MUTE) {
      rdmMuteResponse(&c);
      return;
    } else if (c.packet.PID == E120_DISC_UN_MUTE) {
      // There shouldn't be a response to un mute commands
      return;
    } 
  }

  if (_dmx->rdmCallBack != NULL)
    _dmx->rdmCallBack(&_dmx->rdm_response);
}

void espDMX::rdmEnable(uint16_t ManID, uint32_t DevID) {
  if (_dmx == 0 || _dmx->dirPin == 255 || dmx_input)
    return;
  

    // RDM Variables
  _dmx->rx_pos = 0;
  _dmx->rdm_trans_no = 0;
  _dmx->rdm_discovery = false;
  _dmx->rdm_last_discovery = 0;
  _dmx->todManID = NULL;
  _dmx->todDevID = NULL;
  _dmx->tod_size = 0;
  _dmx->tod_status = RDM_TOD_NOT_READY;
  _dmx->rdm_discovery_pos = 0;
  _dmx->rdmCallBack = NULL;

  _dmx->rdm_enable = true;
  _dmx->rdm_queue.init();

  // Setup direction pin
  digitalWrite(_dmx->dirPin, HIGH);
  
  // Enable RX pin (same for both universes)
  pinMode(3, SPECIAL);
  
  _dmx->rdm_source_man = ManID;
  _dmx->rdm_source_dev = DevID;

  rdmDiscovery();
}

void espDMX::rdmDisable() {
  if (_dmx == 0)
    return;

  if (rdmInUse && rxUser == _dmx->dmx_nr) {
    rdmInUse = false;
    _dmx->state = DMX_STOP;
  }

  _dmx->rdm_enable = false;
  
  digitalWrite(_dmx->dirPin, HIGH);
}

uint8_t espDMX::todStatus() {
  if (_dmx == 0 || !_dmx->rdm_enable)
    return false;
  
  return _dmx->tod_status;
}

uint16_t espDMX::todCount() {
  if (_dmx == 0 || !_dmx->rdm_enable)
    return false;
  
  return _dmx->tod_size;
}


uint16_t* espDMX::todMan() {
  if (_dmx == 0 || !_dmx->rdm_enable)
    return NULL;

  return _dmx->todManID;
}

uint32_t* espDMX::todDev() {
  if (_dmx == 0 || !_dmx->rdm_enable)
    return NULL;

  return _dmx->todDevID;
}

uint16_t espDMX::todMan(uint16_t n) {
  if (_dmx == 0 || !_dmx->rdm_enable)
    return NULL;

  return _dmx->todManID[n];
}

uint32_t espDMX::todDev(uint16_t n) {
  if (_dmx == 0 || !_dmx->rdm_enable)
    return NULL;

  return _dmx->todDevID[n];
}


void espDMX::rdmSetCallBack(rdmCallBackFunc callback) {
  if (_dmx == 0)
    return;
    
  _dmx->rdmCallBack = callback;
}

void espDMX::todSetCallBack(todCallBackFunc callback) {
  if (_dmx == 0)
    return;
    
  _dmx->todCallBack = callback;
}

bool espDMX::rdmEnabled() {
  if (_dmx == 0)
    return 0;
  return _dmx->rdm_enable;
}

void rdmPause(bool p) {
  if (dmx_input && p == false)
    return;

  rdm_pause = p;

  if (p) {
    if (rdmInUse) {
      if (rxUser == 0)
        dmxA.rdmRXTimeout();
      else
        dmxB.rdmRXTimeout();
    }
    rdmInUse = false;
  } else {
    dmxA.rdmDiscovery(RDM_DISCOVERY_FULL);
    dmxB.rdmDiscovery(RDM_DISCOVERY_FULL);
  }
}


void espDMX::dmxIn(bool doIn) {
  if (_dmx == 0)
    return;

  if (doIn) {
    _dmx->isInput = true;

    // Clear our buffers
    memset(_dmx->data, 0, 512);
    memset(_dmx->data1, 0, 512);

    dmx_interrupt_disarm(_dmx);
    rdmPause(true);
      
    // Turn RX pin into UART mode
    pinMode(3, SPECIAL);
    
    // If dirPin is specified then set to in direction
    if (_dmx->dirPin != 255) {
      pinMode(_dmx->dirPin, OUTPUT);
      digitalWrite(_dmx->dirPin, LOW);
    }

    // Set txPin to idle
    digitalWrite(_dmx->txPin, HIGH);

    dmx_input = true;
    rxUser = _dmx->dmx_nr;
    _dmx->state = DMX_RX_IDLE;
  
    noInterrupts();

    // UART at 250k for DMX data
    USD(0) = (ESP8266_CLOCK / DMX_TX_BAUD);
    USC0(0) = DMX_TX_CONF;
    USC1(0) = (1 << UCFFT);	// RX Fifo full threshold
  
    rx_flush();                   // flush rx buffer
    USIC(0) = 0x1ff;              // clear all interrupt flags
  
    // Enable RX Fifo Full, Break Detect & Frame Error Interupts
    USIE(0) |= (1 << UIFF) | (1 << UIBD) | (1 << UIFR);

    // Attach out interupt handler function
    ETS_UART_INTR_ATTACH(&dmx_interrupt_handler, NULL);

    // Enable UART Interrupts
    ETS_UART_INTR_ENABLE();

    interrupts();

  } else {

    // Disable RX Fifo Full, Break Detect & Frame Error Interupts
    USIE(0) &= ~((1 << UIFF) | (1 << UIBD) | (1 << UIFR));
    
    if (_dmx->dirPin != 255) {
      pinMode(_dmx->dirPin, OUTPUT);
      digitalWrite(_dmx->dirPin, HIGH);
    }

    // Clear output buffer & reset channel count
    memset(_dmx->data, 0, 512);
    memset(_dmx->data1, 0, 512);
    _dmx->numChans = 0;
    
    _dmx->isInput = false;
    dmx_input = false;
    rdmPause(false);
  }
}

void espDMX::setInputCallback(inputCallBackFunc callback) {
  if (_dmx == 0)
    return;
  
  _dmx->inputCallBack = callback;
}

void ICACHE_RAM_ATTR espDMX::dmxReceived(uint8_t c) {
  switch ( _dmx->state ) {
  
    case DMX_RX_BREAK:
      if ( c == 0 ) {                     //start code == zero (DMX)
        _dmx->state = DMX_RX_DATA;
        _dmx->numChans = _dmx->rx_pos;
        _dmx->rx_pos = 0;
      } else {
        _dmx->state = DMX_RX_IDLE;
      }
      break;
      
    case DMX_RX_DATA:
      _dmx->data1[_dmx->rx_pos++] = c;
      if ( _dmx->rx_pos >= 512 ) {
        _dmx->state = DMX_RX_IDLE;      // go to idle, wait for next break
      }
      break;
  }
}

void ICACHE_RAM_ATTR espDMX::inputBreak(void) {
  if (_dmx == 0)
    return;
  
  _dmx->state = DMX_RX_BREAK;

  // Double buffer switch
  byte* tmp = _dmx->data;
  _dmx->data = _dmx->data1;
  _dmx->data1 = _dmx->data;

  if (_dmx->inputCallBack)
    _dmx->inputCallBack(_dmx->numChans);
}


void espDMX::handler() {
  if (_dmx == 0 || _dmx->state == DMX_NOT_INIT)
    return;

  // Check if RDM reply should be finished yet
  if (rdmInUse && rxUser == _dmx->dmx_nr && rdmTimer < micros())
    rdmRXTimeout();

  // If DMX is in use then we don't need to proceed
  if (_dmx->state != DMX_STOP)
    return;


  dmx_interrupt_disarm(_dmx);

    // Check if we need to do RDM
    if (!rdm_pause && _dmx->rdm_enable && !rdmInUse) {

      // If we haven't finished our TOD, this will check for any remaining devices
      // or if it's a while since RDM, continue with incremental discovery
      if (_dmx->rdm_queue.isEmpty() && (_dmx->tod_status == RDM_TOD_NOT_READY || (_dmx->rdm_last_discovery + RDM_DISCOVERY_INC_TIME) < millis()))
        rdmDiscovery(RDM_DISCOVERY_INCREMENTAL);


      // Send RDM if there is any
      noInterrupts();
      if (! _dmx->rdm_queue.isEmpty()) {
        rdmInUse = true;
        rx_flush();

        rdm_data* c = _dmx->rdm_queue.peek();
        _dmx->txSize = c->buffer[2] + 3;	// Extra byte added so we don't need a delay in the interrupt

        memcpy(_dmx->data1, c->buffer, _dmx->txSize-1);

        _dmx->state = RDM_START;
      }
      interrupts();

    }

    // If not RDM then do DMX_START
    if (_dmx->state == DMX_STOP && _dmx->started) {

      // If no new DMX and we're not needing to send a full universe then exit
      //if (millis() < _dmx->full_uni_time && !_dmx->newDMX)
      //  return;

      // DMX Transmit
      if (millis() >= _dmx->full_uni_time)
        _dmx->txSize = 512;
      else
        _dmx->txSize = _dmx->numChans;

      // If we are sending a full universe then reset the timer
      if (_dmx->txSize == 512)
        _dmx->full_uni_time = millis() + DMX_FULL_UNI_TIMING;

      // Copy data into the tx buffer
      memcpy(_dmx->data1, _dmx->data, _dmx->txSize);

      _dmx->state = DMX_START;
    }

    if (_dmx->state == DMX_STOP)
      return;

    // Wait for empty FIFO
    while (((USS(_dmx->dmx_nr) >> USTXC) & 0xff) != 0)
      yield();

    // Allow last channel to be fully sent
    delayMicroseconds(44);
    
    // BREAK of ~120us
    pinMode(_dmx->txPin, OUTPUT);
    digitalWrite(_dmx->txPin, LOW);
    delayMicroseconds(118);
    
    // MAB of ~12us
    digitalWrite(_dmx->txPin, HIGH);
    delayMicroseconds(7);
    
    // Change pin to UART mode
    pinMode(_dmx->txPin, SPECIAL);
    
    // Empty FIFO
    dmx_flush(_dmx);

    if (_dmx->state == DMX_START) {

      _dmx->newDMX = false;
      _dmx->state = DMX_TX;
      _dmx->last_dmx_time = millis();
      _dmx->txChan = 0;

      // Set TX Fifo empty trigger point
      USC1(_dmx->dmx_nr) = (50 << UCFET);

      // DMX Start Code 0
      USF(_dmx->dmx_nr) = 0;
      
    } else if (_dmx->state == RDM_START) {

      _dmx->state = RDM_TX;
      rdmTimer = micros() + 5000;
      _dmx->txChan = 1;		// start code is already in the buffer

      // Set TX Fifo empty trigger point & RX Fifo full threshold
      USC1(_dmx->dmx_nr) = (0 << UCFET) | (127 << UCFFT);

      // RDM Start Code 0xCC
      USF(_dmx->dmx_nr) = 0xCC;
    }

    fillTX();
}

void espDMX::fillTX(void) {
  uint16_t fifoRoom = dmx_get_tx_fifo_room(_dmx) - 3;

  uint16_t txSize = _dmx->txSize - _dmx->txChan;
  txSize = (txSize > fifoRoom) ? fifoRoom : txSize;   

  for(; txSize; --txSize)
    USF(_dmx->dmx_nr) = _dmx->data1[_dmx->txChan++];

  dmx_interrupt_arm(_dmx);
}



/* from stream class
int espDMX::available(void) { return 0; }
int espDMX::peek(void) { return 0; }
int espDMX::read(void) { return 0; }
void espDMX::flush(void) { return; }
size_t espDMX::write(uint8_t) { return 0; }

espDMX::operator bool() const {
    return _dmx != 0;
}
*/