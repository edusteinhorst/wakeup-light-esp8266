#ifndef HEADER_DIMMER
  #define HEADER_DIMMER

void initDimmer();
void setDimmer(int percent);
int getDimmer();
void fade(int percent, int duration);
void loopDimmer();

#endif  
