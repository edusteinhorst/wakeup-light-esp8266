#include <Arduino.h>

const int CLAPPER = 5; // on ESP12 is mislabeled as 4

const int CLAP_HYSTERESIS = 200;
const int CLAP_INT_MIN = 300; // minimum interval between claps
const int CLAP_INT_MAX = 500; // maximum interval between claps
const int CLAP_CHECK_INT = 1000; // minimum interval between clap count checks

int lastClap = 0;
int clapCount = 0;

boolean clapperEnabled = false;

void initClapper(){
  
  pinMode(CLAPPER, INPUT); 
}
void clapISR(){

  int elapsed = millis() - lastClap;
  
  // ignore if still hearing the same clap
  if (elapsed < CLAP_HYSTERESIS)
    return;

  if (elapsed > CLAP_INT_MAX){
    clapCount = 1;
  }else if (elapsed > CLAP_INT_MIN){
    clapCount++;
  }

  lastClap = millis();
}

void haltClapper(){

  if (clapperEnabled)
    detachInterrupt(CLAPPER);
}

void resumeClapper(){

  if (clapperEnabled)
    attachInterrupt(CLAPPER, clapISR, RISING);
}

void enableClapper(){

  attachInterrupt(CLAPPER, clapISR, RISING);
  clapperEnabled = true;
}

void disableClapper(){
  
  detachInterrupt(CLAPPER);
  clapperEnabled = false;
}

boolean isClapperEnabled(){

  return clapperEnabled;
}

boolean isClapsEqual(int claps){

  int elapsed = millis() - lastClap;

  if (elapsed > CLAP_CHECK_INT && clapCount == claps){
    clapCount = 0;
    return true;
  }

  return false;
}
