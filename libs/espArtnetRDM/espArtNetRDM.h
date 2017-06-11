
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



#ifndef espArtNetRDM_h
#define espArtNetRDM_h

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
extern "C" {
#include "mem.h"
}
#include "rdmDataTypes.h"
#include "artnet.h"
#include "e131.h"


typedef void (*artDMXCallBack)(uint8_t, uint8_t, uint16_t, bool);
typedef void (*artSyncCallBack)(void);
typedef void (*artRDMCallBack)(uint8_t, uint8_t, rdm_data*);
typedef void (*artIPCallBack)(void);
typedef void (*artAddressCallBack)(void);
typedef void (*artTodRequestCallBack)(uint8_t, uint8_t);
typedef void (*artTodFlushCallBack)(uint8_t, uint8_t);

enum port_type {
  DMX_OUT = 0,
  RDM_OUT = 1,
  DMX_IN = 2
};

struct _port_def {
  // DMX out/in or RDM out
  uint8_t portType;

  // sACN settings
  bool e131;
  uint16_t e131Uni;
  uint16_t e131Sequence;
  uint8_t e131Priority;

  // Port universe
  byte portUni;
  
  // DMX final values buffer
  byte* dmxBuffer;
  uint16_t dmxChans;
  bool ownBuffer;
  bool mergeHTP;
  bool merging;

  // ArtDMX input buffers for 2 IPs
  byte* ipBuffer;
  uint16_t ipChans[2];

  // IPs for current data + time of last packet
  IPAddress senderIP[2];
  unsigned long lastPacketTime[2];

  // IPs for the last 5 RDM commands
  IPAddress rdmSenderIP[5];
  unsigned long rdmSenderTime[5];

  // RDM Variables
  bool todAvailable;
  uint16_t uidTotal;
  uint16_t uidMan[50];
  uint32_t uidSerial[50];
  unsigned long lastTodCommand;
};

typedef struct _port_def port_def;

struct _group_def {
  // Port Address
  byte netSwitch = 0x00;
  byte subnet = 0x00;
  
  port_def* ports[4] = {0,0,0,0};
  byte numPorts = 0;

  IPAddress cancelMergeIP;
  bool cancelMerge;
  unsigned long cancelMergeTime;
};

typedef struct _group_def group_def;

struct _artnet_def {
  
  IPAddress deviceIP;
  IPAddress subnet;
  IPAddress broadcastIP;
  IPAddress rdmIP[5];
  uint8_t rdmIPcount;

  IPAddress syncIP;
  unsigned long lastSync;
  
  uint8_t deviceMAC[6];
  bool dhcp = true;

  char shortName[ARTNET_SHORT_NAME_LENGTH];
  char longName[ARTNET_LONG_NAME_LENGTH];

  byte oemHi;
  byte oemLo;
  byte estaHi;
  byte estaLo;

  group_def* group[16];
  uint8_t numGroups;
  uint32_t lastIPProg;
  uint32_t nextPollReply;

  uint16_t firmWareVersion;
  uint32_t nodeReportCounter;
  uint16_t nodeReportCode;
  char nodeReport[ARTNET_NODE_REPORT_LENGTH];
  
  // callback functions
  artDMXCallBack dmxCallBack = 0;
  artSyncCallBack syncCallBack = 0;
  artRDMCallBack rdmCallBack = 0;
  artIPCallBack ipCallBack = 0;
  artAddressCallBack addressCallBack = 0;
  artTodRequestCallBack todRequestCallBack = 0;
  artTodFlushCallBack todFlushCallBack = 0;
};

typedef struct _artnet_def artnet_device;




class esp8266ArtNetRDM {
  public:
    // init fuctions
    esp8266ArtNetRDM();
    ~esp8266ArtNetRDM();
    
    void init(IPAddress, IPAddress, bool, char*, char*, uint16_t, uint16_t, uint8_t*);
    void init(IPAddress ip, IPAddress sub, bool dhcp, uint16_t oem, uint16_t esta, uint8_t* mac) {
      init(ip, sub, dhcp, "espArtNetNode", "espArtNetNode", oem, esta, mac);
    };
    void init(char* shortName, char* longName, uint16_t oem, uint16_t esta, uint8_t* mac) {
      init(INADDR_NONE, INADDR_NONE, false, shortName, longName, oem, esta, mac);
      setDefaultIP();
    };
    void init(char* shortName, uint16_t oem, uint16_t esta, uint8_t* mac) {
      init(INADDR_NONE, INADDR_NONE, false, shortName, shortName, oem, esta, mac);
      setDefaultIP();
    };
    void init(uint16_t oem, uint16_t esta, uint8_t* mac) {
      init(INADDR_NONE, INADDR_NONE, false, "espArtNetNode", "espArtNetNode", oem, esta, mac);
      setDefaultIP();
    };
    
    void setFirmwareVersion(uint16_t);
    void setDefaultIP();

    uint8_t addGroup(byte, byte);

    uint8_t addPort(byte, byte, byte, uint8_t, bool, byte*);
    uint8_t addPort(byte group, byte port, byte universe, uint8_t type, bool htp) {
      return addPort(group, port, universe, type, htp, 0);
    };
    uint8_t addPort(byte group, byte port, byte universe, uint8_t type) {
      return addPort(group, port, universe, type, true, 0);
    };
    uint8_t addPort(byte group, byte port, byte universe) {
      return addPort(group, port, universe, DMX_OUT, true, 0);
    };

    bool closePort(uint8_t, uint8_t);
    void begin();
    void end();
    void pause();
    byte* getDMX(uint8_t, uint8_t);
    uint16_t numChans(uint8_t, uint8_t);

    // sACN functions
    void setE131(uint8_t, uint8_t, bool);
    bool getE131(uint8_t, uint8_t);
    void setE131Uni(uint8_t, uint8_t, uint16_t);
    
    // handler function for including in loop()
    void handler();

    // set callback functions
    void setArtDMXCallback(void (*dmxCallBack)(uint8_t, uint8_t, uint16_t, bool));
    void setArtRDMCallback(void (*rdmCallBack)(uint8_t, uint8_t, rdm_data*));
    void setArtSyncCallback(void (*syncCallBack)());
    void setArtIPCallback(void (*ipCallBack)());
    void setArtAddressCallback(void (*addressCallBack)());
    void setTODRequestCallback(void (*artTodRequestCallBack)(uint8_t, uint8_t));
    void setTODFlushCallback(void (*artTodFlushCallBack)(uint8_t, uint8_t));

    // set ArtNet uni settings
    void setNet(uint8_t, uint8_t);
    void setSubNet(uint8_t, uint8_t);
    void setUni(uint8_t, uint8_t, uint8_t);
    void setPortType(uint8_t, uint8_t, uint8_t);

    // get ArtNet uni settings
    byte getNet(uint8_t);
    byte getSubNet(uint8_t);
    byte getUni(uint8_t, uint8_t);

    // set network settings
    void setIP(IPAddress, IPAddress);
    void setIP(IPAddress ip) {
      setIP(ip, INADDR_NONE);
    }
    void setDHCP(bool);

    // Set Merge & node name
    void setMerge(uint8_t, uint8_t, bool);
    bool getMerge(uint8_t, uint8_t);
    void setShortName(char*);
    char* getShortName();
    void setLongName(char*);
    char* getLongName();

    // RDM functions
    void rdmResponse(rdm_data*, uint8_t, uint8_t);
    void artTODData(uint8_t, uint8_t, uint16_t*, uint32_t*, uint16_t, uint8_t);
    
    // get network settings
    IPAddress getIP();
    IPAddress getSubnetMask();
    bool getDHCP();

    void setNodeReport(char*, uint16_t);
    void artPollReply();

    void sendDMX(uint8_t, uint8_t, IPAddress, uint8_t*, uint16_t);

  private:
    artnet_device* _art = 0;
    
    int _artOpCode(unsigned char*);
    void _artIPProgReply();
    
    // handlers for received packets
    void _artPoll(void);
    void _artDMX(unsigned char*);
    void _saveDMX(unsigned char*, uint16_t, uint8_t, uint8_t, IPAddress, uint16_t);
    void _artIPProg(unsigned char*);
    void _artAddress(unsigned char*);
    void _artSync(unsigned char*);
    void _artFirmwareMaster(unsigned char*);
    void _artTODRequest(unsigned char*);
    void _artTODControl(unsigned char*);
    void _artRDM(unsigned char*, uint16_t);
    void _artRDMSub(unsigned char*);

    void _e131Receive(e131_packet_t*);
    
    uint8_t _dmxSeqID = 0;
    uint8_t e131Count = 0;	// the number of e131 ports currently open

    WiFiUDP eUDP;
    WiFiUDP fUDP;
};


#endif
