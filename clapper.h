#ifndef HEADER_CLAPPER
  #define HEADER_CLAPPER
  
void initClapper();
void clapISR();
void haltClapper();
void resumeClapper();
void enableClapper();
void disableClapper();
boolean isClapperEnabled();
boolean isClapsEqual(int claps);

#endif
