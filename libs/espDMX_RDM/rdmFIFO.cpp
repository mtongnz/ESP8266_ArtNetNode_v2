#include "rdmFIFO.h"

rdmFIFO::rdmFIFO(void) {
  init();
}

void rdmFIFO::init() {
  RDMfifoMaxSize = RDM_MAX_RDM_QUEUE;
  RDMfifoAllocated = 0;
  RDMfifoSize = 0;
}

bool rdmFIFO::push(rdm_data* a) {
  rdm_data* b;
  b = resize(RDMfifoSize + 1);
  
  if (b == NULL)
    return false;
    
  memcpy(b, a, sizeof(rdm_data));

  // Do endian flip if its backwards
  if (b->buffer[0] == E120_SC_SUB_MESSAGE && b->buffer[1] == E120_SC_RDM) {
    b->endianFlip();
  }

  
  return true;
}

rdm_data* rdmFIFO::peek() {
  if (isEmpty())
    return NULL;

  return content[0];
}

bool rdmFIFO::pop(rdm_data* a) {
  if (isEmpty())
    return false;

  // Data stored in buffer big endian - esp8266 is little endian
  content[0]->endianFlip();
  
  memcpy(a, content[0], sizeof(rdm_data));
  
  resize(RDMfifoSize - 1);
  
  return true;
}

bool rdmFIFO::isEmpty() {
  return (RDMfifoSize == 0);
}

bool rdmFIFO::notEmpty() {
  return (RDMfifoSize != 0);
}

bool rdmFIFO::isFull() {
  return (RDMfifoSize == RDMfifoMaxSize);
}

uint8_t rdmFIFO::count() {
  return RDMfifoSize;
}

uint8_t rdmFIFO::space() {
  return (RDMfifoMaxSize - RDMfifoSize);
}

rdm_data* rdmFIFO::resize(uint8_t s) {
  if (s > RDMfifoMaxSize)
    return NULL;

  if (s < RDMfifoSize) {
    //free(content[0]);
    rdm_data* a = content[0];
    
    for (uint8_t x = 1; x < RDMfifoSize; x++)
      content[x-1] = content[x];

    content[RDMfifoSize-1] = a;
  }

  
  if (s > RDMfifoAllocated) {  // RDMfifoSize) {
    if (!(content[s-1] = (rdm_data*)malloc(sizeof(rdm_data))))
      return false;
    RDMfifoAllocated = s;
  }

  RDMfifoSize = s;
  
  return (content[s-1]);
}

void rdmFIFO::empty() {
  RDMfifoSize = 0;
}
