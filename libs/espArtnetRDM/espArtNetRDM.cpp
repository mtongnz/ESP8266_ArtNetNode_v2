
/*
espArtNetRDM v1 (pre-release) library
Copyright (c) 2016, Matthew Tong
https://github.com/mtongnz/
Modified from https://github.com/forkineye/E131/blob/master/E131.h
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.
If not, see http://www.gnu.org/licenses/
*/



#include "espArtNetRDM.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
extern "C" {
#include "mem.h"
}



void _artClearDMXBuffer(byte* buf);


void _artClearDMXBuffer(byte* buf) {
  memset(buf, 0, DMX_BUFFER_SIZE);
  //for (uint16_t x = 0; x < DMX_BUFFER_SIZE; x++)
  //  buf[x] = 0;
}

esp8266ArtNetRDM::esp8266ArtNetRDM() {
}

esp8266ArtNetRDM::~esp8266ArtNetRDM() {
  end();
}

void esp8266ArtNetRDM::end() {
  if (_art == 0)
    return;

  eUDP.stopAll();

  for (uint8_t g = 0; g < _art->numGroups; g++) {
    for (uint8_t p = 0; p < 4; p++) {
      if (_art->group[g]->ports[p] == 0)
        continue;

      if (_art->group[g]->ports[p]->ownBuffer)
        os_free(_art->group[g]->ports[p]->dmxBuffer);

      os_free(_art->group[g]->ports[p]->ipBuffer);
      os_free(_art->group[g]->ports[p]);
    }
    os_free(_art->group[g]);
  }
  os_free(_art);

  _art = 0;
}

void esp8266ArtNetRDM::init(IPAddress ip, IPAddress subnet, bool dhcp, char* shortname, char* longname, uint16_t oem, uint16_t esta, uint8_t* mac) {
  if (_art != 0)
    os_free(_art);

  // Allocate memory for our settings
  _art = (artnet_device*) os_malloc(sizeof(artnet_device));

  delay(1);
  
  // Store values
  _art->firmWareVersion = 0;
  _art->numGroups = 0;
  _art->nodeReportCounter = 0;
  _art->nodeReportCode = ARTNET_RC_POWER_OK;
  _art->deviceIP = ip;
  _art->subnet = ip;
  _art->broadcastIP = IPAddress((uint32_t)ip | ~((uint32_t)subnet));
  _art->dhcp = dhcp;
  _art->oemLo = (byte)oem;
  _art->oemHi = (byte)(oem >> 8);
  _art->estaLo = (byte)esta;
  _art->estaHi = (byte)(esta >> 8);
  _art->syncIP = INADDR_NONE;
  _art->lastSync = 0;
  _art->nextPollReply = 0;
  memcpy(_art->shortName, shortname, ARTNET_SHORT_NAME_LENGTH);
  memcpy(_art->longName, longname, ARTNET_LONG_NAME_LENGTH);
  memcpy(_art->deviceMAC, mac, 6);
}

void esp8266ArtNetRDM::setFirmwareVersion(uint16_t fw) {
  if (_art == 0)
    return;

  _art->firmWareVersion = fw;
}

void esp8266ArtNetRDM::setDefaultIP() {
  if (_art == 0)
    return;

  _art->dhcp = false;
  _art->subnet = IPAddress(255, 0, 0, 0);
  _art->broadcastIP = IPAddress(2, 255, 255, 255);

  byte b = _art->deviceMAC[3] + _art->oemLo + _art->oemHi;
  byte c = _art->deviceMAC[4];
  byte d = _art->deviceMAC[5];

  _art->deviceIP = IPAddress(2, b, c, d);
}

uint8_t esp8266ArtNetRDM::addGroup(byte net, byte subnet) {
  if (_art == 0)
    return 255;

  uint8_t g = _art->numGroups;
  
  _art->group[g] = (group_def*) os_malloc(sizeof(group_def));
  _art->group[g]->netSwitch = net & 0b01111111;
  _art->group[g]->subnet = subnet;
  _art->group[g]->numPorts = 0;
  _art->group[g]->cancelMergeIP = INADDR_NONE;
  _art->group[g]->cancelMerge = 0;
  _art->group[g]->cancelMergeTime = 0;

  for (int x = 0; x < 4; x++)
    _art->group[g]->ports[x] = 0;
  
  _art->numGroups++;

  return g;
}

uint8_t esp8266ArtNetRDM::addPort(byte g, byte p, byte universe, uint8_t t, bool htp, byte* buf) {
  if (_art == 0)
    return 255;

  // Check for a valid universe, group and port number
  if (universe > 15 || p >= 4 || g > _art->numGroups)
    return 255;
  
  group_def* group = _art->group[g];
  
  // Check if port is already initialised, return its port number
  if (group->ports[p] != 0)
    return p;

  // Allocate space for our port
  group->ports[p] = (port_def*) os_malloc(sizeof(port_def));
  
  delay(1);
  port_def* port = group->ports[p];
  
  // DMX output buffer allocation
  if (buf == 0) {
    port->dmxBuffer = (byte*) os_malloc(DMX_BUFFER_SIZE);
    port->ownBuffer = true;
  } else {
    port->dmxBuffer = buf;
    port->ownBuffer = false;
  }

  // Clear the buffer
  _artClearDMXBuffer(port->dmxBuffer);

  
  // Store settings
  group->numPorts++;
  port->portType = t;
  port->mergeHTP = htp;
  port->portUni = universe;
  port->senderIP[0] = INADDR_NONE;
  port->senderIP[1] = INADDR_NONE;

  for (uint8_t x = 0; x < 5; x++)
    port->rdmSenderIP[x] = INADDR_NONE;

  port->ipBuffer = 0;
  port->ipChans[0] = 0;
  port->ipChans[1] = 0;
  port->dmxChans = 0;
  port->merging = 0;
  port->lastTodCommand = 0;
  port->uidTotal = 0;  
  port->todAvailable = 0;
  
  return p;
}

bool esp8266ArtNetRDM::closePort(uint8_t g, uint8_t p) {
  if (_art == 0 || g >= _art->numGroups)
    return 0;
  
  group_def* group = _art->group[g];
  
  // Port already closed
  if (group->ports[p] == 0)
    return true;

  // Delete buffers
  if (group->ports[p]->ownBuffer)
    os_free(group->ports[p]->dmxBuffer);
  if (group->ports[p]->ipBuffer != 0)
    os_free(group->ports[p]->ipBuffer);
  
  os_free(group->ports[p]);

  // Mark port as empty
  group->ports[p] = 0;
  group->numPorts--;
  group->ports[p] == 0;
}

void esp8266ArtNetRDM::setArtDMXCallback(artDMXCallBack callback) {
  if (_art == 0)
    return;

  _art->dmxCallBack = callback;
}

void esp8266ArtNetRDM::setArtSyncCallback(artSyncCallBack callback) {
  if (_art == 0)
    return;

  _art->syncCallBack = callback;
}

void esp8266ArtNetRDM::setArtRDMCallback(artRDMCallBack callback) {
  if (_art == 0)
    return;

  _art->rdmCallBack = callback;
}

void esp8266ArtNetRDM::setArtIPCallback(artIPCallBack callback) {
  if (_art == 0)
    return;

  _art->ipCallBack = callback;
}

void esp8266ArtNetRDM::setArtAddressCallback(artAddressCallBack callback) {
  if (_art == 0)
    return;

  _art->addressCallBack = callback;
}

void esp8266ArtNetRDM::setTODRequestCallback(artTodRequestCallBack callback) {
  if (_art == 0)
    return;

  _art->todRequestCallBack = callback;
}

void esp8266ArtNetRDM::setTODFlushCallback(artTodFlushCallBack callback) {
  if (_art == 0)
    return;

  _art->todFlushCallBack = callback;
}
    
void esp8266ArtNetRDM::begin() {
  if (_art == 0)
    return;

  // Start listening for UDP packets
  eUDP.begin(ARTNET_PORT);
  eUDP.flush();
  fUDP.begin(E131_PORT);
  fUDP.flush();
  
  // Send ArtPollReply to tell everyone we're here
  artPollReply();
}
    
void esp8266ArtNetRDM::pause() {
  if (_art == 0)
    return;

  eUDP.flush();
  eUDP.stopAll();
}

void esp8266ArtNetRDM::handler() {
  if (_art == 0)
    return;

  // Artnet packet
  uint16_t packetSize = eUDP.parsePacket();

  if(packetSize > 0) {

    unsigned char _artBuffer[ARTNET_BUFFER_MAX];

    // Read data into buffer
    eUDP.read(_artBuffer, packetSize);
  
    // Get the Op Code
    int opCode = _artOpCode(_artBuffer);

    switch (opCode) {

      case ARTNET_ARTPOLL:
	// This is always called at the end of this function
        //_artPoll();
        break;

      case ARTNET_ARTDMX:
        _artDMX(_artBuffer);
        break;

      case ARTNET_IP_PROG:
        _artIPProg(_artBuffer);
        break;
  
      case ARTNET_ADDRESS:
        _artAddress(_artBuffer);
        break;
  
      case ARTNET_SYNC:
        _artSync(_artBuffer);
        break;
  
      case ARTNET_FIRMWARE_MASTER:
        _artFirmwareMaster(_artBuffer);
        break;
  
      case ARTNET_TOD_REQUEST:
        _artTODRequest(_artBuffer);
        break;
  
      case ARTNET_TOD_CONTROL:
        _artTODControl(_artBuffer);
        break;
  
      case ARTNET_RDM:
        _artRDM(_artBuffer, packetSize);
        break;
  
      case ARTNET_RDM_SUB:
        _artRDMSub(_artBuffer);
        break;
    }
  }


  // e131 packet
  packetSize = fUDP.parsePacket();

  if(packetSize > 0) {

    e131_packet_t _e131Buffer;

    // Read data into buffer
    fUDP.readBytes(_e131Buffer.raw, packetSize);

    _e131Receive(&_e131Buffer);
  }

  // Send artPollReply - the function will limit the number sent
  _artPoll();

}

int esp8266ArtNetRDM::_artOpCode(unsigned char *_artBuffer) {
  String test = String((char*)_artBuffer);
  if ( test.equals("Art-Net") ) {
    if ( _artBuffer[11] >= 14 ) {                 //protocol version [10] hi byte [11] lo byte
      return _artBuffer[9] *256 + _artBuffer[8];  //opcode lo byte first
    }
  }
  
  return 0;
}


void esp8266ArtNetRDM::_artPoll() {
  // limit the number of artPollReply messages
  if (_art->nextPollReply > millis())
    return;
  _art->nextPollReply = millis() + 2000;
  
  unsigned char _artReplyBuffer[ARTNET_REPLY_SIZE];
  _artReplyBuffer[0] = 'A';
  _artReplyBuffer[1] = 'r';
  _artReplyBuffer[2] = 't';
  _artReplyBuffer[3] = '-';
  _artReplyBuffer[4] = 'N';
  _artReplyBuffer[5] = 'e';
  _artReplyBuffer[6] = 't';
  _artReplyBuffer[7] = 0;
  _artReplyBuffer[8] = ARTNET_ARTPOLL_REPLY;      	// op code lo-hi
  _artReplyBuffer[9] = ARTNET_ARTPOLL_REPLY >> 8; 	// 0x2100 = artPollReply
  _artReplyBuffer[10] = _art->deviceIP[0];        	// ip address
  _artReplyBuffer[11] = _art->deviceIP[1];
  _artReplyBuffer[12] = _art->deviceIP[2];
  _artReplyBuffer[13] = _art->deviceIP[3];
  _artReplyBuffer[14] = 0x36;               		// port lo first always 0x1936
  _artReplyBuffer[15] = 0x19;
  _artReplyBuffer[16] = _art->firmWareVersion >> 8;     // firmware hi-lo
  _artReplyBuffer[17] = _art->firmWareVersion;
  _artReplyBuffer[20] = _art->oemHi;                    // oem hi-lo
  _artReplyBuffer[21] = _art->oemLo;
  _artReplyBuffer[22] = 0;              		// ubea
	
  _artReplyBuffer[23] = 0b11110010;			// Device is RDM Capable
  _artReplyBuffer[24] = _art->estaLo;           	// ESTA Code (2 bytes)
  _artReplyBuffer[25] = _art->estaHi;

                                        //short name
  for (int x = 0; x < ARTNET_SHORT_NAME_LENGTH; x++)
    _artReplyBuffer[x+26] = _art->shortName[x];
    
                                        //long name
  for (int x = 0; x < ARTNET_LONG_NAME_LENGTH; x++)
    _artReplyBuffer[x+44] = _art->longName[x];

                                        // node report - send blank
  for (int x = 0; x < ARTNET_NODE_REPORT_LENGTH; x++) {
    _artReplyBuffer[x+108] = 0;
  }


  // Set reply code
  char tmp[7];
  sprintf (tmp, "%04x", _art->nodeReportCode);
  _artReplyBuffer[108] = '#';
  _artReplyBuffer[109] = tmp[0];
  _artReplyBuffer[110] = tmp[1];
  _artReplyBuffer[111] = tmp[2];
  _artReplyBuffer[112] = tmp[3];
  _artReplyBuffer[113] = '[';

  // Max 6 digits for counter - could be longer if wanted
  sprintf (tmp, "%d", _art->nodeReportCounter++);
  if (_art->nodeReportCounter > 999999)
    _art->nodeReportCounter = 0;

  // Format counter and add to reply buffer
  uint8_t x = 0;
  for (x = 0; tmp[x] != '\0' && x < 6; x++)
    _artReplyBuffer[x + 114] = tmp[x];

  uint8_t rLen = ARTNET_NODE_REPORT_LENGTH - x - 2;
  x = x + 114;

  _artReplyBuffer[x++] = ']';
  _artReplyBuffer[x++] = ' ';

  // Append plain text report
  for (uint8_t y = 0; y < rLen && _art->nodeReport[y] != '\0'; y++)
    _artReplyBuffer[x++] = _art->nodeReport[y];


  _artReplyBuffer[172] = 0;             //number of ports Hi (always 0)
  _artReplyBuffer[194] = 0;             // these are not used
  _artReplyBuffer[195] = 0;
  _artReplyBuffer[196] = 0;
  _artReplyBuffer[197] = 0;
  _artReplyBuffer[198] = 0;
  _artReplyBuffer[199] = 0;
  _artReplyBuffer[200] = 0;             // Style - 0x00 = DMX to/from Artnet

  for (int x = 0; x < 6; x++)           // MAC Address
    _artReplyBuffer[201 + x] = _art->deviceMAC[x];

  _artReplyBuffer[207] = _art->deviceIP[0];        // bind ip
  _artReplyBuffer[208] = _art->deviceIP[1];
  _artReplyBuffer[209] = _art->deviceIP[2];
  _artReplyBuffer[210] = _art->deviceIP[3];
  
  _artReplyBuffer[212] = (_art->dhcp) ? 31 : 29;  // status 2

  for (int x = 213; x < ARTNET_REPLY_SIZE; x++)
    _artReplyBuffer[x] = 0;             // Reserved for future - transmit 0


  // Set values for each group of ports and send artPollReply
  for (uint8_t groupNum = 0; groupNum < _art->numGroups; groupNum++) {
    group_def* group = _art->group[groupNum];
    
    if (group->numPorts == 0)
      continue;

    _artReplyBuffer[18] = group->netSwitch;       // net
    _artReplyBuffer[19] = group->subnet;          // subnet
    _artReplyBuffer[173] = group->numPorts;       //number of ports (Lo byte)

    _artReplyBuffer[211] = groupNum+1;    	  // Bind Index

    // Port details
    for (int x = 0; x < 4; x++) {

      // Send blank values for empty ports
      _artReplyBuffer[174 + x] = 0;
      _artReplyBuffer[178 + x] = 0;
      _artReplyBuffer[182 + x] = 0;
      _artReplyBuffer[186 + x] = 0;
      _artReplyBuffer[190 + x] = 0;

      // This port isn't in use
      if (group->ports[x] == 0)
        continue;

      // DMX or RDM out port
      if (group->ports[x]->portType != DMX_IN) {

        // Get values for Good Output field
        byte go = 0;
        if (group->ports[x]->dmxChans != 0)
          go |= 128;						// data being transmitted
        if (group->ports[x]->merging)
          go |= 8;						// artnet data being merged
        if (! group->ports[x]->mergeHTP)
          go |= 2;						// Merge mode LTP
        if (group->ports[x]->e131)
          go |= 1;						// sACN
        
        _artReplyBuffer[174 + x] |= 128;			//Port Type (128 = DMX out)
        _artReplyBuffer[182 + x] = go;				//Good output (128 = data being transmitted)
        _artReplyBuffer[190 + x] = group->ports[x]->portUni;  	// swOut - port address

      // DMX In port info
      } else if (group->ports[x]->portType == DMX_IN) {
        _artReplyBuffer[174 + x] |= 64;				// Port type (64 = DMX in)

        if (group->ports[x]->dmxChans != 0)
          _artReplyBuffer[178 + x] = 128;       		// Good input (128 = data being received)

        _artReplyBuffer[186] = group->ports[0]->portUni;  	// swIn

      }
    }

    // Send packet
    eUDP.beginPacket(_art->broadcastIP, ARTNET_PORT);
    eUDP.write(_artReplyBuffer,ARTNET_REPLY_SIZE);
    eUDP.endPacket();

    delay(0);
  }
}


void esp8266ArtNetRDM::artPollReply() {
  if (_art == 0)
    return;

  _artPoll();
}

void esp8266ArtNetRDM::_artDMX(unsigned char *_artBuffer) {
  group_def* group = 0;

  IPAddress rIP = eUDP.remoteIP();

  uint8_t net = (_artBuffer[15] & 0x7F);
  uint8_t sub = (_artBuffer[14] >> 4);
  uint8_t uni = (_artBuffer[14] & 0x0F);

  // Number of channels hi byte first
  uint16_t numberOfChannels = _artBuffer[17] + (_artBuffer[16] << 8);
  uint16_t startChannel = 0;

  // Loop through all groups
  for (int x = 0; x < _art->numGroups; x++) {
    if (net == _art->group[x]->netSwitch && sub == _art->group[x]->subnet) {
      group = _art->group[x];

      // Loop through each port
      for (int y = 0; y < 4; y++) {
        if (group->ports[y] == 0 || group->ports[y]->portType == DMX_IN)
          continue;
        
        // If this port has the correct Net, Sub & Uni then save DMX to buffer
        if (uni == group->ports[y]->portUni)
          _saveDMX(&_artBuffer[ARTNET_ADDRESS_OFFSET], numberOfChannels, x, y, rIP, startChannel);
      }
    }
  }
}

void esp8266ArtNetRDM::_saveDMX(unsigned char *dmxData, uint16_t numberOfChannels, uint8_t groupNum, uint8_t portNum, IPAddress rIP, uint16_t startChannel) {
  group_def* group = _art->group[groupNum];
  port_def* port = group->ports[portNum];

  uint8_t senderID = 255;  // Will be set to 0 or 1 if valid later

  unsigned long timeNow = millis();

  // We can't do the next calculations until after 10 seconds
  if (timeNow > 10000) {
    unsigned long timeExp = timeNow - 10000;

    // Clear IPs that we haven't heard from in over 10 seconds
    if (port->lastPacketTime[0] < timeExp)
      port->senderIP[0] = INADDR_NONE;
    else if (port->lastPacketTime[1] < timeExp)
      port->senderIP[1] = INADDR_NONE;
  }

  // Get a sender ID
  if (port->senderIP[0] == rIP) {
    senderID = 0;
    port->lastPacketTime[0] = timeNow;
  } else if (port->senderIP[1] == rIP || port->senderIP[1] == INADDR_NONE) {
    senderID = 1;
    port->senderIP[1] = rIP;
    port->lastPacketTime[1] = timeNow;
  } else if (port->senderIP[0] == INADDR_NONE) {
    senderID = 0;
    port->senderIP[0] = rIP;
    port->lastPacketTime[0] = timeNow;
  }

  // This is a third IP so drop the packet (Artnet v4 only allows for merging 2 DMX streams)
  if (senderID == 255)
    return;
  
  // Check if we're merging (the other IP will be non zero)
  if (port->senderIP[(senderID ^ 0x01)] == INADDR_NONE)
    port->merging = false;
  else
    port->merging = true;
  

  // Cancel merge is old so cancel the cancel merge
  if ((group->cancelMergeTime + ARTNET_CANCEL_MERGE_TIMEOUT) < millis()) {
    group->cancelMerge = false;
    group->cancelMergeIP = INADDR_NONE;
  
  } else {
    // This is the correct IP, enable cancel merge
    if (group->cancelMergeIP == port->senderIP[senderID]) {
      group->cancelMerge = 1;
      group->cancelMergeTime = millis();
      port->mergeHTP = false;
      port->merging = false;
      
    // If the merge is current & IP isn't correct, ignore this packet
    } else if (group->cancelMerge)
      return;
  }
  
  // Store number of channels
  if (numberOfChannels > port->dmxChans)
    port->dmxChans = numberOfChannels;

  // Check if we should merge (HTP) or not merge (LTP)
  if (port->merging && port->mergeHTP) {
    // Check if there is a buffer.  If not, allocate and clear it
    if (port->ipBuffer == 0) {
      
      port->ipBuffer = (byte*) os_malloc(2 * DMX_BUFFER_SIZE);
      delay(0);
      _artClearDMXBuffer(port->ipBuffer);
      _artClearDMXBuffer(&port->ipBuffer[DMX_BUFFER_SIZE]);
      delay(0);
    }

    // Put data into our buffer
    memcpy(&port->ipBuffer[senderID * DMX_BUFFER_SIZE + startChannel], dmxData, numberOfChannels);
    
    // Get the number of channels to compare
    numberOfChannels = (port->dmxChans > numberOfChannels) ? port->dmxChans : numberOfChannels;
    
    // Compare data and put in the output buffer
    for (uint16_t x = 0; x < numberOfChannels; x++)
      port->dmxBuffer[x] = (port->ipBuffer[x] > port->ipBuffer[x + DMX_BUFFER_SIZE]) ? port->ipBuffer[x] : port->ipBuffer[x + DMX_BUFFER_SIZE];

    // Call our dmx callback in the main script (Sync doesn't get used when merging)
    _art->dmxCallBack(groupNum, portNum, numberOfChannels, false);
    
  } else {
    // Copy data directly into output buffer
    memcpy(&port->dmxBuffer[startChannel], dmxData, numberOfChannels);
    
/*
    // Delete merge buffer if it exists
    if (port->ipBuffer != 0) {
      os_free(port->ipBuffer);
      port->ipBuffer = 0;
    }
*/

    // Check if Sync is enabled and call dmx callback in the main script
    if (_art->lastSync == 0 || (_art->lastSync + 4000) < timeNow || _art->syncIP != rIP)
      _art->dmxCallBack(groupNum, portNum, numberOfChannels, false);
    else
      _art->dmxCallBack(groupNum, portNum, numberOfChannels, true);

//    _art->syncIP = rIP;
  }
}

byte* esp8266ArtNetRDM::getDMX(uint8_t g, uint8_t p) {
  if (_art == 0)
    return NULL;

  if (g < _art->numGroups) {
    if (_art->group[g]->ports[p] != 0)
      return _art->group[g]->ports[p]->dmxBuffer;
  }
  return NULL;
}

uint16_t esp8266ArtNetRDM::numChans(uint8_t g, uint8_t p) {
  if (_art == 0)
    return 0;

  if (g < _art->numGroups) {
    if (_art->group[g]->ports[p] != 0)
      return _art->group[g]->ports[p]->dmxChans;
  }
  return 0;
}

void esp8266ArtNetRDM::_artIPProg(unsigned char *_artBuffer) {
  // Don't do anything if it's the same command again
  if ((_art->lastIPProg + 20) > millis())
    return;
  _art->lastIPProg = millis();
  
  byte command = _artBuffer[14];

  // Enable DHCP
  if ((command & 0b11000000) == 0b11000000) {
    _art->dhcp = true;

  // Disable DHCP
  }else if ((command & 0b11000000) == 0b10000000) {
    _art->dhcp = false;
    
    // Program IP
    if ((command & 0b10000100) == 0b10000100)
      _art->deviceIP = IPAddress(_artBuffer[16], _artBuffer[17], _artBuffer[18], _artBuffer[19]);
      
    // Program subnet
    if ((command & 0b10000010) == 0b10000010) {
      _art->subnet = IPAddress(_artBuffer[20], _artBuffer[21], _artBuffer[22], _artBuffer[23]);
      _art->broadcastIP = IPAddress((uint32_t)_art->deviceIP | ~((uint32_t)_art->subnet));
    }

    // Use default address
    if ((command & 0b10001000) == 0b10001000)
      setDefaultIP();
  }
  
  // Run callback - must be before reply for correct dhcp setting
  if (_art->ipCallBack != 0)
    _art->ipCallBack();
  
  // Send reply
  _artIPProgReply();

  // Send artPollReply
  artPollReply();
}

void esp8266ArtNetRDM::_artIPProgReply() {
  // Initialise our reply
  char ipProgReply[ARTNET_IP_PROG_REPLY_SIZE];
  
  ipProgReply[0] = 'A';
  ipProgReply[1] = 'r';
  ipProgReply[2] = 't';
  ipProgReply[3] = '-';
  ipProgReply[4] = 'N';
  ipProgReply[5] = 'e';
  ipProgReply[6] = 't';
  ipProgReply[7] = 0;
  ipProgReply[8] = ARTNET_IP_PROG_REPLY;      // op code lo-hi
  ipProgReply[9] = ARTNET_IP_PROG_REPLY >> 8; // 0x2100 = artPollReply
  ipProgReply[10] = 0;
  ipProgReply[11] = 14;                 // artNet version (14)
  ipProgReply[12] = 0;
  ipProgReply[13] = 0;
  ipProgReply[14] = 0;
  ipProgReply[15] = 0;
  ipProgReply[16] = _art->deviceIP[0];  // ip address
  ipProgReply[17] = _art->deviceIP[1];
  ipProgReply[18] = _art->deviceIP[2];
  ipProgReply[19] = _art->deviceIP[3];
  ipProgReply[20] = _art->subnet[0];    // subnet address
  ipProgReply[21] = _art->subnet[1];
  ipProgReply[22] = _art->subnet[2];
  ipProgReply[23] = _art->subnet[3];
  ipProgReply[24] = 0;
  ipProgReply[25] = 0;
  ipProgReply[26] = (_art->dhcp)?(1 << 6) : 0;  // DHCP enabled
  ipProgReply[27] = 0;
  ipProgReply[28] = 0;
  ipProgReply[29] = 0;
  ipProgReply[30] = 0;
  ipProgReply[31] = 0;
  ipProgReply[32] = 0;
  ipProgReply[33] = 0;

  // Send packet
  eUDP.beginPacket(eUDP.remoteIP(), ARTNET_PORT);
  int test = eUDP.write(ipProgReply,ARTNET_IP_PROG_REPLY_SIZE);
  eUDP.endPacket();
}

void esp8266ArtNetRDM::_artAddress(unsigned char *_artBuffer) {
  // _artBuffer[13]    bindIndex
  uint8_t g = _artBuffer[13] - 1;

  // Set net switch
  if ((_artBuffer[12] & 0x80) == 0x80)
    _art->group[g]->netSwitch = _artBuffer[12] & 0x7F;
  
  // Set short name
  if (_artBuffer[14] != '\0') {
    for (int x = 0; x < ARTNET_SHORT_NAME_LENGTH; x++)
      _art->shortName[x] = _artBuffer[x + 14];
  }

  // Set long name
  if (_artBuffer[32] != '\0') {
    for (int x = 0; x < ARTNET_LONG_NAME_LENGTH; x++)
      _art->longName[x] = _artBuffer[x + 32];
  }

  // Set Port Address
  for (int x = 0; x < 4; x++) {
    if ((_artBuffer[100 + x] & 0xF0) == 0x80 && _art->group[g]->ports[x] != 0)
      _art->group[g]->ports[x]->portUni = _artBuffer[100 + x] & 0x0F;
  }

  // Set subnet
  if ((_artBuffer[104] & 0xF0) == 0x80) {
    _art->group[g]->subnet = _artBuffer[104] & 0x0F;
  }

  // Get port number
    uint8_t p = _artBuffer[106] & 0x0F;

  // Command
  switch (_artBuffer[106]) {
    case ARTNET_AC_CANCEL_MERGE:
      _art->group[g]->cancelMergeTime = millis();
      _art->group[g]->cancelMergeIP = eUDP.remoteIP();

      /*
      for (int x = 0; x < 4; x++) {
        if (_art->group[g]->ports[x] == 0)
          continue;
        
        // Delete merge buffer if it exists
        if (_art->group[g]->ports[x]->ipBuffer != 0) {
          os_free(_art->group[g]->ports[x]->ipBuffer);
          _art->group[g]->ports[x]->ipBuffer = 0;
        }
        
        // Update our timer variables
        _art->group[g]->ports[x]->lastPacketTime[0] = 0;
        _art->group[g]->ports[x]->lastPacketTime[1] = 0;
      }
      */
      break;
      
    case ARTNET_AC_MERGE_LTP_0:
    case ARTNET_AC_MERGE_LTP_1:
    case ARTNET_AC_MERGE_LTP_2:
    case ARTNET_AC_MERGE_LTP_3:
      if (_art->group[g]->ports[p] != 0) {
        // Delete merge buffer if it exists
        if (_art->group[g]->ports[p]->ipBuffer != 0) {
          os_free(_art->group[g]->ports[p]->ipBuffer);
          _art->group[g]->ports[p]->ipBuffer = 0;
        }
        
        // Update our timer variables
        _art->group[g]->ports[p]->lastPacketTime[0] = 0;
        _art->group[g]->ports[p]->lastPacketTime[1] = 0;
  
        // Set to LTP
        _art->group[g]->ports[p]->mergeHTP = false;
  
        // Cancel the cancel merge
        _art->group[g]->cancelMerge = 0;
        _art->group[g]->cancelMergeIP = INADDR_NONE;
      }
      break;
      
    case ARTNET_AC_MERGE_HTP_0:
    case ARTNET_AC_MERGE_HTP_1:
    case ARTNET_AC_MERGE_HTP_2:
    case ARTNET_AC_MERGE_HTP_3:
      // Set to HTP
      if (_art->group[g]->ports[p] != 0) {
        _art->group[g]->ports[p]->mergeHTP = true;

        // Cancel the cancel merge
        _art->group[g]->cancelMerge = 0;
        _art->group[g]->cancelMergeIP = INADDR_NONE;
      }
      break;
      
    case ARTNET_AC_CLEAR_OP_0:
    case ARTNET_AC_CLEAR_OP_1:
    case ARTNET_AC_CLEAR_OP_2:
    case ARTNET_AC_CLEAR_OP_3:
      if (_art->group[g]->ports[p] == 0) {
        // Delete merge buffer if it exists
        if (_art->group[g]->ports[p]->ipBuffer != 0) {
          os_free(_art->group[g]->ports[p]->ipBuffer);
          _art->group[g]->ports[p]->ipBuffer = 0;
        }

        // Clear the DMX output buffer
        _artClearDMXBuffer(_art->group[g]->ports[p]->dmxBuffer);
      }
      break;


    case ARTNET_AC_ARTNET_SEL_0:
    case ARTNET_AC_ARTNET_SEL_1:
    case ARTNET_AC_ARTNET_SEL_2:
    case ARTNET_AC_ARTNET_SEL_3:
      for (uint8_t x = 0; x < 4; x++) {
        if (_art->group[g]->ports[x] == 0)
          setE131(g, x, false);
      }
      break;

    case ARTNET_AC_ACN_SEL_0:
    case ARTNET_AC_ACN_SEL_1:
    case ARTNET_AC_ACN_SEL_2:
    case ARTNET_AC_ACN_SEL_3:
      for (uint8_t x = 0; x < 4; x++) {
        if (_art->group[g]->ports[p] == 0)
          setE131(g, p, true);
      }
      break;

  }

  // Send reply
  artPollReply();
  
  // Run callback
  if (_art->addressCallBack != 0)
    _art->addressCallBack();
}

void esp8266ArtNetRDM::_artSync(unsigned char *_artBuffer) {
  // Update sync timer
  _art->lastSync = millis();
  
  // Run callback
  if (_art->syncCallBack != 0)// && _art->syncIP == eUDP.remoteIP())
    _art->syncCallBack();
}

void esp8266ArtNetRDM::_artFirmwareMaster(unsigned char *_artBuffer) {
  //Serial.println("artFirmwareMaster");
}

void esp8266ArtNetRDM::_artTODRequest(unsigned char *_artBuffer) {
  byte net = _artBuffer[21];
  group_def* group;

  uint8_t numAddress = _artBuffer[23];
  uint8_t addr = 24;

  // Handle artTodControl requests
  if (_artOpCode(_artBuffer) == ARTNET_TOD_CONTROL) {
    numAddress = 1;
    addr = 23;
  }
  
  for (int g = 0; g < _art->numGroups; g++) {
    group = _art->group[g];
    
    // Net matches so loop through the addresses
    if (group->netSwitch == net) {
      for (int y = 0; y < numAddress; y++) {
        
        // Subnet doesn't match, try the next address
        if (group->subnet != (_artBuffer[addr+y] >> 4))
          continue;

        // Subnet matches so loop through the 4 ports and check universe
        for (int p = 0; p < 4; p++) {
          
          if (group->ports[p] == 0)
            continue;
          
          port_def* port = group->ports[p];
          
          if (port->portUni != (_artBuffer[addr+y] & 0x0F))
            continue;

          port->lastTodCommand = millis();
          
          // Flush TOD
          if (_artBuffer[22] == 0x01)
            _art->todFlushCallBack(g, p);
          
          // TOD Request
          else
            _art->todRequestCallBack(g, p);
        }
      }

      
    }
  }

}

void esp8266ArtNetRDM::artTODData(uint8_t g, uint8_t p, uint16_t* uidMan, uint32_t* uidDev, uint16_t uidTotal, uint8_t state) {
  if (_art == 0)
    return;

  // Initialise our reply
  uint16_t len = ARTNET_TOD_DATA_SIZE + (6 * uidTotal);
  char artTodData[len];
  artTodData[0] = 'A';
  artTodData[1] = 'r';
  artTodData[2] = 't';
  artTodData[3] = '-';
  artTodData[4] = 'N';
  artTodData[5] = 'e';
  artTodData[6] = 't';
  artTodData[7] = 0;
  artTodData[8] = ARTNET_TOD_DATA;      // op code lo-hi
  artTodData[9] = ARTNET_TOD_DATA >> 8;
  artTodData[10] = 0;
  artTodData[11] = 14;                 // artNet version (14)
  artTodData[12] = 0x01;               // rdm standard Ver 1.0
  artTodData[13] = p+1;                // port number (1-4 not 0-3)
  artTodData[14] = 0;
  artTodData[15] = 0;
  artTodData[16] = 0;
  artTodData[17] = 0;
  artTodData[18] = 0;
  artTodData[19] = 0;
  artTodData[20] = g+1;                // bind index
  artTodData[21] = _art->group[g]->netSwitch;

  if (state == RDM_TOD_READY)
    artTodData[22] = 0x00;             // TOD full
  else
    artTodData[22] = 0xFF;             // TOD not avail or incomplete

  artTodData[23] = (_art->group[g]->subnet << 4) | _art->group[g]->ports[p]->portUni;
  artTodData[24] = uidTotal >> 8;      // number of RDM devices found
  artTodData[25] = uidTotal;

  uint8_t blockCount = 0;
  uint16_t uidPos = 0;
  
  uint16_t f = uidTotal;

  while (1) {
    artTodData[26] = blockCount;
    artTodData[27] = (uidTotal > 200) ? 200 : uidTotal;
    
    uint8_t uidCount = 0;

    // Add RDM UIDs (48 bit each) - max 200 per packet
    for (uint16_t xx = 28; uidCount < 200 && uidTotal > 0; uidCount++) {
      uidTotal--;
      
      artTodData[xx++] = uidMan[uidTotal] >> 8;
      artTodData[xx++] = uidMan[uidTotal];
      artTodData[xx++] = uidDev[uidTotal] >> 24;
      artTodData[xx++] = uidDev[uidTotal] >> 16;
      artTodData[xx++] = uidDev[uidTotal] >> 8;
      artTodData[xx++] = uidDev[uidTotal];
    }

    // Send packet
    eUDP.beginPacket(_art->broadcastIP, ARTNET_PORT);
    int test = eUDP.write(artTodData,len);
    eUDP.endPacket();

    if (uidTotal == 0)
      break;

    blockCount++;
  }
}

void esp8266ArtNetRDM::_artTODControl(unsigned char *_artBuffer) {
  _artTODRequest(_artBuffer);
}

void esp8266ArtNetRDM::_artRDM(unsigned char *_artBuffer, uint16_t packetSize) {
  if (_art->rdmCallBack == 0)
    return;

  IPAddress remoteIp = eUDP.remoteIP();

  byte net = _artBuffer[21] * 0x7F;
  byte sub = _artBuffer[23] >> 4;
  byte uni = _artBuffer[23] & 0x0F;

  // Get RDM data into out buffer ready to send
  rdm_data c;
  c.buffer[0] = 0xCC;
  memcpy (&c.buffer[1], &_artBuffer[24], _artBuffer[25] + 2);

  group_def* group = 0;
  unsigned long timeNow = millis();
  
  // Get the group number
  for (int x = 0; x < _art->numGroups; x++) {
    if (net == _art->group[x]->netSwitch && sub == _art->group[x]->subnet) {
      group = _art->group[x];

      // Get the port number
      for (int y = 0; y < 4; y++) {

        // If the port isn't in use
        if (group->ports[y] == 0 || group->ports[y]->portType != RDM_OUT)
          continue;

        // Run callback
        if (uni == group->ports[y]->portUni) {
          _art->rdmCallBack(x, y, &c);

          bool ipSet = false;

          for (int q = 0; q < 5; q++) {
            // Check when last packets where received.  Clear if over 200ms
            if (timeNow >= (group->ports[y]->rdmSenderTime[q] + 200))
              group->ports[y]->rdmSenderIP[q] = INADDR_NONE;
        
            // Save our IP
            if (!ipSet) {
              if (group->ports[y]->rdmSenderIP[q] == INADDR_NONE || group->ports[y]->rdmSenderIP[q] == remoteIp) {
                group->ports[y]->rdmSenderIP[q] = remoteIp;
                group->ports[y]->rdmSenderTime[q] = timeNow;
                ipSet = true;
              }
            }
          }
        }
      }
    }
  }
}

void esp8266ArtNetRDM::rdmResponse(rdm_data* c, uint8_t g, uint8_t p) {
  if (_art == 0)
    return;

  uint16_t len = ARTNET_RDM_REPLY_SIZE + c->packet.Length + 1;
  // Initialise our reply
  char rdmReply[len];
  
  rdmReply[0] = 'A';
  rdmReply[1] = 'r';
  rdmReply[2] = 't';
  rdmReply[3] = '-';
  rdmReply[4] = 'N';
  rdmReply[5] = 'e';
  rdmReply[6] = 't';
  rdmReply[7] = 0;
  rdmReply[8] = ARTNET_RDM;          // op code lo-hi
  rdmReply[9] = ARTNET_RDM >> 8;
  rdmReply[10] = 0;
  rdmReply[11] = 14;                 // artNet version (14)
  rdmReply[12] = 0x01;               // RDM version - RDM STANDARD V1.0

  for (uint8_t x = 13; x < 21; x++)
    rdmReply[x] = 0;

  rdmReply[21] = _art->group[g]->netSwitch;
  rdmReply[22] = 0x00;              // Command - 0x00 = Process RDM Packet
  rdmReply[23] = (_art->group[g]->subnet << 4) | _art->group[g]->ports[p]->portUni;

  // Copy everything except the 0xCC start code
  memcpy(&rdmReply[24], &c->buffer[1], c->packet.Length + 1);

  for (int x = 0; x < 5; x++) {
    if (_art->group[g]->ports[p]->rdmSenderIP[x] != INADDR_NONE) {
      // Send packet
      eUDP.beginPacket(_art->group[g]->ports[p]->rdmSenderIP[x], ARTNET_PORT);
      int test = eUDP.write(rdmReply,len);
      eUDP.endPacket();
    }
  }
}

void esp8266ArtNetRDM::_artRDMSub(unsigned char *_artBuffer) {
  //Serial.println("artRDMSub");
}

IPAddress esp8266ArtNetRDM::getIP() {
  if (_art == 0)
    return INADDR_NONE;
  return _art->deviceIP;
}

IPAddress esp8266ArtNetRDM::getSubnetMask() {
  if (_art == 0)
    return INADDR_NONE;
  return _art->subnet;
}

bool esp8266ArtNetRDM::getDHCP() {
  if (_art == 0)
    return 0;
  return _art->dhcp;
}


void esp8266ArtNetRDM::setIP(IPAddress ip, IPAddress subnet) {
  if (_art == 0)
    return;
  _art->deviceIP = ip;
  
  if ( (uint32_t)subnet != 0 )
    _art->subnet = subnet;
  
  _art->broadcastIP = IPAddress((uint32_t)_art->deviceIP | ~((uint32_t)_art->subnet));
}

void esp8266ArtNetRDM::setDHCP(bool d) {
  if (_art == 0)
    return;
  _art->dhcp = d;
}

void esp8266ArtNetRDM::setNet(uint8_t g, uint8_t net) {
  if (_art == 0 || g >= _art->numGroups)
    return;
  _art->group[g]->netSwitch = net;
}

uint8_t esp8266ArtNetRDM:: getNet(uint8_t g) {
  if (_art == 0 || g >= _art->numGroups)
    return 0;
  return _art->group[g]->netSwitch;
}

void esp8266ArtNetRDM::setSubNet(uint8_t g, uint8_t sub) {
  if (_art == 0 || g >= _art->numGroups)
    return;
  _art->group[g]->subnet = sub;
}

byte esp8266ArtNetRDM::getSubNet(uint8_t g) {
  if (_art == 0 || g >= _art->numGroups)
    return 0;
  return _art->group[g]->subnet;
}

void esp8266ArtNetRDM::setUni(uint8_t g, uint8_t p, uint8_t uni) {
  if (_art == 0 || g >= _art->numGroups || _art->group[g]->ports[p] == 0)
    return;
  _art->group[g]->ports[p]->portUni = uni;
}

byte esp8266ArtNetRDM::getUni(uint8_t g, uint8_t p) {
  if (_art == 0 || g >= _art->numGroups || _art->group[g]->ports[p] == 0)
    return 0;
  return _art->group[g]->ports[p]->portUni;
}


void esp8266ArtNetRDM:: setPortType(uint8_t g, uint8_t p, uint8_t t) {
  if (_art == 0 || g >= _art->numGroups || _art->group[g]->ports[p] == 0)
    return;

  _art->group[g]->ports[p]->portType = t;
}

void esp8266ArtNetRDM::setMerge(uint8_t g, uint8_t p, bool htp) {
  if (_art == 0 || g >= _art->numGroups || _art->group[g]->ports[p] == 0)
    return;
  _art->group[g]->ports[p]->mergeHTP = htp;
}

bool esp8266ArtNetRDM::getMerge(uint8_t g, uint8_t p) {
  if (_art == 0 || g >= _art->numGroups || _art->group[g]->ports[p] == 0)
    return 0;
  return _art->group[g]->ports[p]->mergeHTP;
}



void esp8266ArtNetRDM::setShortName(char* name) {
  if (_art == 0)
    return;
  memcpy(_art->shortName, name, ARTNET_SHORT_NAME_LENGTH);
}

char* esp8266ArtNetRDM::getShortName() {
  if (_art == 0)
    return NULL;
  return _art->shortName;
}


void esp8266ArtNetRDM::setLongName(char* name) {
  if (_art == 0)
    return;
  memcpy(_art->longName, name, ARTNET_LONG_NAME_LENGTH);
}

char* esp8266ArtNetRDM::getLongName() {
  if (_art == 0)
    return NULL;
  return _art->longName;
}

void esp8266ArtNetRDM::setNodeReport(char* c, uint16_t code) {
  if (_art == 0)
    return;

  strcpy(_art->nodeReport, c);
  _art->nodeReportCode = code;
}

void esp8266ArtNetRDM::sendDMX(uint8_t g, uint8_t p, IPAddress bcAddress, uint8_t* data, uint16_t length) {
  if (_art == 0 || _art->numGroups <= g || _art->group[g]->ports[p] == 0)
    return;

  uint8_t net = _art->group[g]->netSwitch;
  uint8_t subnet = _art->group[g]->subnet;
  uint8_t uni = _art->group[g]->ports[p]->portUni;

  // length is always even and up to 512 channels
  if (length % 2)
    length += 1;
  if (length > 512)
    length = 512;

  _art->group[g]->ports[p]->dmxChans = length;

  unsigned char _artDMX[ARTNET_BUFFER_MAX];
  _artDMX[0] = 'A';
  _artDMX[1] = 'r';
  _artDMX[2] = 't';
  _artDMX[3] = '-';
  _artDMX[4] = 'N';
  _artDMX[5] = 'e';
  _artDMX[6] = 't';
  _artDMX[7] = 0;
  _artDMX[8] = ARTNET_ARTDMX;      	// op code lo-hi
  _artDMX[9] = ARTNET_ARTDMX >> 8;	
  _artDMX[10] = 0;  		   	// protocol version (14)
  _artDMX[11] = 14;
  _artDMX[12] = _dmxSeqID++;		// sequence ID
  _artDMX[13] = p;		   	// Port ID (not really necessary)
  _artDMX[14] = (subnet << 4) | uni;	// Subuni
  _artDMX[15] = (net & 0x7F);		// Netswitch
  _artDMX[16] = (length >> 8);		// DMX Data length
  _artDMX[17] = (length & 0xFF);

  for (uint16_t x = 0; x < length; x++)
    _artDMX[18 + x] = data[x];

  // Send packet
  eUDP.beginPacket(bcAddress, ARTNET_PORT);
  eUDP.write(_artDMX,(18 + length));
  eUDP.endPacket();

}

void esp8266ArtNetRDM::setE131(uint8_t g, uint8_t p, bool a) {
  if (_art == 0 || _art->numGroups <= g || _art->group[g]->ports[p] == 0)
    return;

  // Increment or decrement our e131Count variable
  if (!_art->group[g]->ports[p]->e131 && a) {
    e131Count+=1;

    // Clear the DMX output buffer
    _artClearDMXBuffer(_art->group[g]->ports[p]->dmxBuffer);

  } else if (_art->group[g]->ports[p]->e131 && !a) {
    e131Count-=1;

    // Clear the DMX output buffer
    _artClearDMXBuffer(_art->group[g]->ports[p]->dmxBuffer);
  }

  _art->group[g]->ports[p]->e131 = a;
}

bool esp8266ArtNetRDM::getE131(uint8_t g, uint8_t p) {
  if (_art == 0 || _art->numGroups <= g || _art->group[g]->ports[p] == 0 || _art->group[g]->ports[p]->e131 == false)
    return false;

  return true;
}

void esp8266ArtNetRDM::setE131Uni(uint8_t g, uint8_t p, uint16_t u) {
  if (_art == 0 || _art->numGroups <= g || _art->group[g]->ports[p] == 0)
    return;

  _art->group[g]->ports[p]->e131Uni = u;
  _art->group[g]->ports[p]->e131Sequence = 0;
  _art->group[g]->ports[p]->e131Priority = 0;
}

void esp8266ArtNetRDM::_e131Receive(e131_packet_t* e131Buffer) {
  if (_art == 0 || _art->numGroups == 0 || e131Count == 0)
    return;

  // Check for sACN packet errors.  Error reporting not implemented -> just dump packet

  if (memcmp(e131Buffer->acn_id, ACN_ID, sizeof(e131Buffer->acn_id)))
    //return ERROR_ACN_ID;
    return;

  if (__builtin_bswap32(e131Buffer->root_vector) != VECTOR_ROOT)
    //return ERROR_VECTOR_ROOT;
    return;

  if (__builtin_bswap32(e131Buffer->frame_vector) != VECTOR_FRAME)
    //return ERROR_VECTOR_FRAME;
    return;

  if (e131Buffer->dmp_vector != VECTOR_DMP)
    //return ERROR_VECTOR_DMP;
    return;


  // No errors -> continue with sACN processing


  uint16_t uni = (e131Buffer->universe << 8) | ((e131Buffer->universe >> 8) & 0xFF);
  uint16_t numberOfChannels = (e131Buffer->property_value_count << 8) | ((e131Buffer->property_value_count >> 8) & 0xFF) - 1;
  uint16_t startChannel = (e131Buffer-> first_address << 8) | ((e131Buffer-> first_address >> 8) & 0xFF);
  uint16_t seq = e131Buffer->sequence_number;

  uint8_t _e131Count = 0;

  group_def* group = 0;

  IPAddress rIP = fUDP.remoteIP();

  // Loop through all groups
  for (int x = 0; x < _art->numGroups; x++) {
      group = _art->group[x];

      // Loop through each port
      for (int y = 0; y < 4; y++) {
        if (group->ports[y] == 0 || group->ports[y]->portType == DMX_IN || !group->ports[y]->e131)
          continue;
        
        // If this port has the correct Uni, is a later packet, and is of a valid priority -> save DMX to buffer
        if (uni == group->ports[y]->e131Uni && seq > group->ports[y]->e131Sequence && e131Buffer->priority >= group->ports[y]->e131Priority) {

          // Drop non-zero start packets
          if (e131Buffer->property_values[0] != 0)
            continue;

          // A higher priority will override previous data - this is handled in saveDMX but we need to clear the IPs & buffer
          if (e131Buffer->priority > group->ports[y]->e131Priority) {
            _artClearDMXBuffer(group->ports[y]->dmxBuffer);
            group->ports[y]->senderIP[0] = INADDR_NONE;
            group->ports[y]->senderIP[1] = INADDR_NONE;
          }

          group->ports[y]->e131Priority = e131Buffer->priority;

          _saveDMX(&e131Buffer->property_values[1], numberOfChannels, x, y, rIP, startChannel);
        }

        // If all the e131 ports are checked, then return
        if (e131Count == ++_e131Count)
          return;
      }
  }

}





