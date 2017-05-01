#ifndef rdmDataTypes_h
#define rdmDataTypes_h

#define byte uint8_t

enum rdm_tod_state {
  RDM_TOD_NOT_READY,
  RDM_TOD_READY,
  RDM_TOD_ERROR
};

union rdm_data_ {
  struct {
    uint16_t StartCode;    // Start Code 0xCC01 for RDM
    byte     Length;       // packet length
    uint16_t DestMan;
    uint32_t DestDev;
    uint16_t SourceMan;
    uint32_t SourceDev;
    byte     TransNo;      // transaction number, not checked
    byte     ResponseType; // ResponseType
    byte     MsgCount;     // Message count
    uint16_t SubDev;       // sub device number (root = 0) 
    byte     CmdClass;     // command class
    uint16_t PID;          // parameter ID
    byte     DataLength;   // parameter data length in bytes
    byte     Data[231];    // data byte field
  } __attribute__((packed)) packet;
  
  struct {
    byte headerFE;
    byte headerAA;
    byte maskedDevID[12];
    byte maskedChecksum[4];
  } __attribute__((packed)) discovery;
  
  byte buffer[255];

  void endianFlip(void) {
    // 16 bit flips
    packet.StartCode = (packet.StartCode << 8) | (packet.StartCode >> 8);
    packet.DestMan = (packet.DestMan << 8) | (packet.DestMan >> 8);
    packet.SourceMan = (packet.SourceMan << 8) | (packet.SourceMan >> 8);
    packet.SubDev = (packet.SubDev << 8) | (packet.SubDev >> 8);
    packet.PID = (packet.PID << 8) | (packet.PID >> 8);

    // 32 bit flips
    packet.DestDev = __builtin_bswap32 (packet.DestDev);
    packet.SourceDev = __builtin_bswap32 (packet.SourceDev);
  }

  void clear(void) {
    memset(&buffer, 0, 255);
    //for (uint8_t x = 0; x < 255; x++)
    //  buffer[x] = 0;
  }
};
typedef union rdm_data_ rdm_data;

#endif
