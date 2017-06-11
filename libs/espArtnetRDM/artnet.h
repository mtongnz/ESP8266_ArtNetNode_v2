
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



#ifndef artnet_data_h
#define artnet_data_h

#define ARTNET_PORT 6454
#define ARTNET_BUFFER_MAX 600
#define ARTNET_REPLY_SIZE 239
#define ARTNET_IP_PROG_REPLY_SIZE 34
#define ARTNET_RDM_REPLY_SIZE 24
#define ARTNET_TOD_DATA_SIZE 28
#define ARTNET_ADDRESS_OFFSET 18
#define ARTNET_SHORT_NAME_LENGTH 18
#define ARTNET_LONG_NAME_LENGTH 64
#define ARTNET_NODE_REPORT_LENGTH 64
#define ARTNET_CANCEL_MERGE_TIMEOUT 2500
#define DMX_BUFFER_SIZE 512
#define DMX_MAX_CHANS 512

// Artnet Op Codes
#define ARTNET_ARTPOLL 0x2000
#define ARTNET_ARTPOLL_REPLY 0x2100
#define ARTNET_DIAG_DATA 0x2300
#define ARTNET_COMMAND 0x2400
#define ARTNET_ARTDMX 0x5000
#define ARTNET_NZS 0x5100
#define ARTNET_SYNC 0x5200
#define ARTNET_ADDRESS 0x6000
#define ARTNET_INPUT 0x7000
#define ARTNET_TOD_REQUEST 0x8000
#define ARTNET_TOD_DATA 0x8100
#define ARTNET_TOD_CONTROL 0x8200
#define ARTNET_RDM 0x8300
#define ARTNET_RDM_SUB 0x8400
#define ARTNET_FIRMWARE_MASTER 0xF200
#define ARTNET_FIRMWARE_REPLY 0xF300
#define ARTNET_IP_PROG 0xF800
#define ARTNET_IP_PROG_REPLY 0xF900

// Artnet Node Report Codes
#define ARTNET_RC_DEBUG 0x0000
#define ARTNET_RC_POWER_OK 0x0001
#define ARTNET_RC_POWER_FAIL 0x0002
#define ARTNET_RC_SH_NAME_OK 0x0006
#define ARTNET_RC_LO_NAME_OK 0x0007
#define ARTNET_RC_FIRMWARE_FAIL 0x000E

// Artnet Command Codes
#define ARTNET_AC_NONE 0x00
#define ARTNET_AC_CANCEL_MERGE 0x01
#define ARTNET_AC_LED_NORMAL 0x02
#define ARTNET_AC_LED_MUTE 0x03
#define ARTNET_AC_LED_LOCATE 0x04
#define ARTNET_AC_RESET_RX_FLAGS 0x05
#define ARTNET_AC_MERGE_LTP_0 0x10
#define ARTNET_AC_MERGE_LTP_1 0x11
#define ARTNET_AC_MERGE_LTP_2 0x12
#define ARTNET_AC_MERGE_LTP_3 0x13
#define ARTNET_AC_MERGE_HTP_0 0x50
#define ARTNET_AC_MERGE_HTP_1 0x51
#define ARTNET_AC_MERGE_HTP_2 0x52
#define ARTNET_AC_MERGE_HTP_3 0x53
#define ARTNET_AC_CLEAR_OP_0 0x90
#define ARTNET_AC_CLEAR_OP_1 0x91
#define ARTNET_AC_CLEAR_OP_2 0x92
#define ARTNET_AC_CLEAR_OP_3 0x93
#define ARTNET_AC_ARTNET_SEL_0 0x60
#define ARTNET_AC_ARTNET_SEL_1 0x61
#define ARTNET_AC_ARTNET_SEL_2 0x62
#define ARTNET_AC_ARTNET_SEL_3 0x63
#define ARTNET_AC_ACN_SEL_0 0x70
#define ARTNET_AC_ACN_SEL_1 0x71
#define ARTNET_AC_ACN_SEL_2 0x72
#define ARTNET_AC_ACN_SEL_3 0x73

#endif
