#ifndef rdmFIFO_h
#define rdmFIFO_h

#include "Arduino.h"
#include "rdm.h"
#include "rdmDataTypes.h"

#define RDM_MAX_RDM_QUEUE 30

class rdmFIFO {
  public:
    rdmFIFO();
    void init();
    bool push(rdm_data* a);
    rdm_data* peek(void);
    bool pop(rdm_data* a);
    bool isEmpty(void);
    bool notEmpty(void);
    bool isFull(void);
    uint8_t count(void);
    uint8_t space(void);
    void empty(void);

  private:
    rdm_data* resize(uint8_t s);

    rdm_data* content[RDM_MAX_RDM_QUEUE];
    uint8_t RDMfifoMaxSize;
    uint8_t RDMfifoAllocated;
    uint8_t RDMfifoSize;
};


#endif
