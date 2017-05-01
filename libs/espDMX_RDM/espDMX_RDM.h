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

#ifndef espDMX_h
#define espDMX_h

#define DMX_MAX_BYTES_PER_INT 3		// How many bytes to send per interrupt
#define DMX_TX_CONF           0x3c   	// SERIAL_8N2
#define DMX_TX_BAUD           250000
#define DMX_FULL_UNI_TIMING   800   	// How often to output full 512 channel universe (in milliseconds)
#define DMX_NO_LED            200
#define DMX_MIN_CHANS         30     	// Minimum channels output = this + DMX_ADD_CHANS
#define DMX_ADD_CHANS         30     	// Add extra buffer to the number of channels output
#define UART_TX_FIFO_SIZE     0x80

#define RDM_DISCOVERY_INC_TIME    700       // How often to run incremental discovery
#define RDM_DISCOVERY_INCREMENTAL 0
#define RDM_DISCOVERY_FULL        1
#define RDM_DISCOVERY_TOD_WIPE    2


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
extern "C" {
#include "osapi.h"
#include "ets_sys.h"
#include "mem.h"
#include "user_interface.h"
}
#include <inttypes.h>
#include "Stream.h"


#include "rdm.h"
#include "rdmDataTypes.h"
#include "rdmFIFO.h"


typedef void (*rdmCallBackFunc)(rdm_data*);
typedef void (*todCallBackFunc)(void);
typedef void (*inputCallBackFunc)(uint16_t);


// DMX states
enum dmx_state {
  DMX_STOP,
  DMX_START,
  DMX_TX,
  DMX_NOT_INIT,
  RDM_START,
  RDM_TX,
  RDM_RX,
  DMX_RX_BREAK,
  DMX_RX_DATA,
  DMX_RX_IDLE
};

union byte_uint64 {
  byte b[8];
  uint64_t u;
};


struct dmx_ {
    uint8_t dmx_nr;
    uint8_t txPin;
    uint8_t dirPin;
    uint8_t ledIntensity;
    uint8_t state = DMX_NOT_INIT;

    uint16_t numChans;
    uint16_t txChan;
    uint16_t txSize;

    long full_uni_time;
    long last_dmx_time;
    long led_timer;
    bool newDMX = false;
    bool started = false;

    byte* data;
    byte* data1;
    bool ownBuffer = 0;

    bool isInput = false;
    inputCallBackFunc inputCallBack = NULL;

    bool rdm_enable = false;
    rdmFIFO rdm_queue;
    rdm_data rdm_response;
    uint16_t rx_pos = 0;
    uint16_t rdm_source_man;
    uint32_t rdm_source_dev;
    uint8_t rdm_trans_no = 0;
    bool rdm_discovery = false;
    uint32_t rdm_last_discovery = 0;
    uint16_t* todManID = NULL;
    uint32_t* todDevID = NULL;
    uint16_t tod_size = 0;
    uint8_t tod_status = RDM_TOD_NOT_READY;
    uint16_t rdm_discovery_pos = 0;
    bool tod_changed = false;

    rdmCallBackFunc rdmCallBack = NULL;
    todCallBackFunc todCallBack = NULL;
};
typedef struct dmx_ dmx_t;

class espDMX {
    public:
        espDMX(uint8_t dmx_nr);
        ~espDMX();

  	void begin(uint8_t dir, byte* buf);
	void begin(uint8_t dir) {
      		begin(dir, NULL);
  	};
	void begin(byte* buf) {
      		begin(255, buf);
  	};
	void begin(void) {
      		begin(255, NULL);
  	};

	void setBuffer(byte*);
	void setBuffer(void) {
		setBuffer(NULL);
	};
        void pause();
        void unPause();
        void end();
        void ledIntensity(uint8_t);
        
        void setChans(byte *data) {
            setChans(data, 512, 1);
        }
        void setChans(byte *data, uint16_t numChans) {
            setChans(data, numChans, 1);
        }
        void setChans(byte*, uint16_t, uint16_t);


  	void chanUpdate(uint16_t);
        void clearChans();
        byte *getChans();
        uint16_t numChans();
        
/*  from stream class
        int available(void) override;
        int peek(void) override;
        int read(void) override;
        void flush(void) override;
        size_t write(uint8_t) override;
        operator bool() const;
*/

        void rdmEnable(uint16_t, uint32_t);
        void rdmDisable(void);
        void rdmDiscovery(uint8_t);
        void rdmDiscovery() {
          rdmDiscovery(RDM_DISCOVERY_TOD_WIPE);
        };
        
        void rdmSetCallBack(void (*rdmCallBackFunc)(rdm_data*));
        void todSetCallBack(void (*todCallBackFunc)(void));
        
        bool rdmSendCommand(rdm_data*);
        bool rdmSendCommand(uint8_t, uint16_t, uint16_t, uint32_t, byte*, uint16_t, uint16_t);
        bool rdmSendCommand(uint8_t cmdClass, uint16_t pid, uint16_t manID, uint32_t devID, byte* data, uint16_t dataLength) {
          return rdmSendCommand(cmdClass, pid, manID, devID, data, dataLength, 0);
        };
        bool rdmSendCommand(uint8_t cmdClass, uint16_t pid, uint16_t manID, uint32_t devID) {
          return rdmSendCommand(cmdClass, pid, manID, devID, NULL, 0, 0);
        };

  bool rdmEnabled(void);
        uint8_t todStatus(void);
        uint16_t todCount(void);

        uint16_t* todMan(void);
        uint32_t* todDev(void);
        uint16_t todMan(uint16_t n);
        uint32_t todDev(uint16_t n);

        void handler(void);

        void dmxIn(bool doIn);

        void setInputCallback(void (*inputCallBackFunc)(uint16_t));
        
    private:
        friend void dmx_interrupt_handler(void);
        friend void rdm_timer_handler(void);
        friend void rdm_interrupt_disarm(dmx_t* dmx);
        friend void rdmPause(bool);
        
        void _transmit(void);
	void fillTX(void);
        
        void inputBreak(void);
        void dmxReceived(uint8_t);

        void rdmRXTimeout(void);
        void rdmBreakDetect(void);
        
        void rdmReceived(void);
        void rdmMuteResponse(rdm_data*);
        void rdmDiscoveryResponse(rdm_data*);
        
        bool rdmDiscConfirm();
        
        bool rdmDiscoverBranch(uint16_t, uint32_t, uint16_t, uint32_t, bool);
        bool rdmDiscoverBranch(void) {
          return rdmDiscoverBranch(0x0000, 0x00000000, 0xFFFF, 0xFFFFFFFF, false);
        };

        uint8_t _dmx_nr;
        dmx_t* _dmx;
};


extern espDMX dmxA;
extern espDMX dmxB;
extern void rdmPause(bool);
#endif

