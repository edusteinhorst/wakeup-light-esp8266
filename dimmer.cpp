#include <Arduino.h>
#include <TimeLib.h>

// gamma correction table. Max output value is 941 which is 92% duty, because my circuit keeps the FET 100% on at this level
const int myGamma[] = {
    0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  3,  4,  5,
    6,  7,  8,  9, 10, 12, 14, 15, 17, 19, 22, 24, 27, 29, 32, 35,
   39, 42, 46, 50, 54, 58, 63, 67, 72, 78, 83, 89, 94,101,107,114,
  121,128,135,143,151,159,168,176,186,195,205,215,225,236,247,258,
  270,282,294,307,320,333,347,361,375,390,405,420,436,453,469,486,
  504,522,540,558,578,597,617,637,658,679,701,723,745,768,791,815,
  839,864,889,915,941};

const int DIMMER = 14;
const int MIN_DUTY = 0;

int currentDimmer = 0;
int fadeDuration = 0; // fade in/ou duration in ms
long fadeStart = 0;
int fadeTo = 0;
int fadeFrom = 0;
boolean fading = false;

void initDimmer(){

  pinMode(DIMMER, OUTPUT);
  analogWrite(DIMMER, MIN_DUTY);
  analogWriteFreq(400);
}

void setDimmer(int percent){

  if (percent < 0 || percent > 100)
    return;
  
  currentDimmer = percent;
  
  int absValue  = myGamma[percent];
  analogWrite(DIMMER, absValue);
}

int getDimmer(){

  return currentDimmer;
}

void fade(int percent, int duration){

  fadeStart = millis();
  fadeTo = percent;
  fadeFrom = currentDimmer;
  fadeDuration = duration;
}

void loopDimmer(){

  long msNow = millis();

  // update dimmer until a little bit after fade to guarantee we reach 100%
  if (msNow > fadeStart && msNow < (fadeStart + fadeDuration + 2000)){
    int range = abs(fadeTo - fadeFrom);
    int percentageElapsed = ((msNow - fadeStart) * 100) / fadeDuration;
    // clamp at 100%
    if (percentageElapsed > 100)
      percentageElapsed = 100;

    int delta = (range * percentageElapsed) / 100;
    if (fadeTo > fadeFrom){
      setDimmer(fadeFrom + delta);
    }else{
      setDimmer(fadeFrom - delta);
    }
  }
}

