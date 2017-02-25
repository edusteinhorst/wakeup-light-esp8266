#include <Arduino.h>
#include "clapper.h"

const int BUZZER = 4; // on ESP12 is mislabeled as 5
const int MUD = 80; // morse unit duration in ms

void initBuzzer(){
  
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
}

void dit(int times){

  for (int i = times; i > 0; i--){
    digitalWrite(BUZZER, LOW);
    delay(MUD);
    digitalWrite(BUZZER, HIGH);
    delay(MUD);    
  }

}

void dah(int times){

  for (int i = times; i > 0; i--){
    digitalWrite(BUZZER, LOW);
    delay(MUD * 3);
    digitalWrite(BUZZER, HIGH);    
    delay(MUD);
  }
   
}

void letterGap(){
  delay(MUD * 2); // not 3x because each dit/dah has a MUD after it
}

void wordGap(){
  delay(MUD * 6); // not 7x because each dit/dah has a MUD after it
}

void soundAlarm() {

  haltClapper();
  dit(12);
  resumeClapper();
}

void beepAck(){
  haltClapper();
  dah(2);
  resumeClapper();  
}

void easterEgg(){

  haltClapper();
  
  // B
  dah(1);
  dit(3);
  letterGap();
  // O
  dah(3);
  letterGap();
  // M
  dah(2);
  wordGap();
  
  // D
  dah(1);
  dit(2);
  letterGap();
  // I
  dit(2);
  letterGap();
  // A
  dit(1);
  dah(1);
  wordGap();
  
  dah(1); // X
  dit(2);
  dah(1);
  letterGap();
  dit(2); // U
  dah(1);
  letterGap();
  dah(1); // X
  dit(2);
  dah(1);
  letterGap();
  dit(2); // U
  dah(1);  
  wordGap();
  
  // L
  dit(1);
  dah(1);
  dit(2);
  letterGap();
  // I
  dit(2);
  letterGap();
  // N
  dah(1);
  dit(1);
  letterGap();
  // D
  dah(1);
  dit(2);
  letterGap();
  // A
  dit(1);
  dah(1);
  wordGap();
  // ,
  dah(2);
  dit(2);
  dah(2);
  wordGap();
  
  dah(1); // T
  letterGap();
  dit(1); // E
  wordGap();
  dit(1); // A
  dah(1); 
  letterGap();
  dah(2); // M
  letterGap();
  dah(3); // O
  
  resumeClapper();  
  
}

void soundSOS(){

    haltClapper();
    dit(3);
    letterGap();
    dah(3);
    letterGap();
    dit(3);
    resumeClapper();  
}

