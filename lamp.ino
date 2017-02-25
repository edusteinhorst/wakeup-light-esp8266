#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <TimeAlarms.h> //  modify lib header file if need more than 6 alarms: #define dtNBR_ALARMS 7
#include <WebSocketsServer.h>
#include <Hash.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include "buzzer.h"
#include "clapper.h"
#include "dimmer.h"

const int TIME_SYNC_INTERVAL = 600; // how many seconds between clock synchronizations with internet provider
const int SUNRISE_START_BEFORE_ALARM = 1800; // how many seconds before alarm for sunrise to start
const int SUNRISE_DURATION = 1800000; // how many milliseconds total sunrise duration 

const int STATE_EEPROM_ADDR = 0x00;
const int MAX_STATE_LENGTH = 92;

const int BUTTON = 0; // button gpio
const int BUTTON_HYSTERESIS = 1000;

const char* timeDBHost = "api.timezonedb.com";
const char* timeDBPath = "/?zone=America/Sao_Paulo&format=json&key=YOUR_KEY_GOES_HERE"; // timezone and DST service, create an account

int lastButtonPush = 0;
int buttonPushed = false; 

int alarmOn = true;
int sunriseOn = true;
int beepingOn = true;
bool alarmDays[] = {false, false, false, false, false, false, false, false};
int alarmHour = 0;
int alarmMin = 0;
int lastDimmer = 0;

String lastSavedStateString = "";

int alarmID = 0;
int sunriseAlarmID = 0;
int soundAlarmID = 0;
boolean soundingAlarm = false;

WebSocketsServer webSocket = WebSocketsServer(81);

void alarmTime(){
  
  if (alarmOn && beepingOn && alarmDays[weekday() - 1] == true){
    soundingAlarm = true;
    if (random(1,21) == 1){
      soundAlarmID = Alarm.timerRepeat(30,easterEgg);        
    }else{
      soundAlarmID = Alarm.timerRepeat(15,soundAlarm);        
    }
  }
}

void sunriseTime(){

  if (alarmOn && alarmDays[weekday() - 1] == true && sunriseOn){
    Serial.println("Starting sunrise");
    fade(100, SUNRISE_DURATION);
  }
}

void setAlarmTo(int hours, int mins){

  Alarm.free(alarmID);
  Alarm.free(sunriseAlarmID);
  alarmID = Alarm.alarmRepeat(hours,mins,00,alarmTime);
  sunriseAlarmID = Alarm.alarmRepeat(Alarm.read(alarmID) - SUNRISE_START_BEFORE_ALARM, sunriseTime);
}

String getStateString(){
  
  String state = "CLAPPER=";
  state += isClapperEnabled();
  state += "&ALARM=";
  state += alarmOn;
  state += "&DIMMER=";
  state += lastDimmer;
  state += "&SUNRISE=";
  state += sunriseOn;
  state += "&BEEPING=";
  state += beepingOn;
  state += "&ALARM_TIME=";
  if (alarmHour < 10)
    state += "0";  
  state += alarmHour;
  state += ':';
  if (alarmMin < 10)
    state += "0";
  state += alarmMin; 
  state += "&ALARM_DAYS=";
  if (alarmDays[0] == true)
    state += "Su";
  if (alarmDays[1] == true)
    state += "Mo";
  if (alarmDays[2] == true)
    state += "Tu";
  if (alarmDays[3] == true)
    state += "We";
  if (alarmDays[4] == true)
    state += "Th";
  if (alarmDays[5] == true)
    state += "Fr";
  if (alarmDays[6] == true)
    state += "Sa";

  return state;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch (type) {
    case WStype_DISCONNECTED:

      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        // send eeprom state back to user device
        String state = getStateString();                                                         
        webSocket.sendTXT(num, state);

        String clockString = "CLOCK=";
        clockString += hour();
        clockString += ":";
        if (minute() < 10)
          clockString += "0";
        clockString += minute();
        clockString += ":";
        if (second() < 10)
          clockString += "0";
        clockString += second();                    
   
        webSocket.sendTXT(num, clockString);        
      }
      break;
    case WStype_TEXT:
      {

        String text = String((char *) &payload[0]);

        if (text.startsWith("CLAPPER=")){
          String val = (text.substring(text.indexOf("=") + 1, text.length()));
          bool clapperOn = val.toInt();
          if (clapperOn){
            enableClapper();
            Serial.println("Clapper enabled");
          }else{
            disableClapper();
            Serial.println("Clapper disabled");
          }
        }

        if (text.startsWith("ALARM=")){
          String val = (text.substring(text.indexOf("=") + 1, text.length()));
          alarmOn = val.toInt();
          if (alarmOn){
            Serial.println("Alarm enabled");
          }else{
            Serial.println("Alarm disabled");
          }
        }        

        if (text.startsWith("DIMMER=")) {

          String val = (text.substring(text.indexOf("=") + 1, text.length()));
          int dimmer = val.toInt();

            setDimmer(dimmer);
            lastDimmer = getDimmer();         
            Serial.print("Dimmer set to ");
            Serial.println(dimmer);
        }

        if (text.startsWith("SUNRISE=")){
          String val = (text.substring(text.indexOf("=") + 1, text.length()));
          sunriseOn = val.toInt();
          if (sunriseOn){
            Serial.println("Sunrise enabled");
          }else{
            Serial.println("Sunrise disabled");
          }
        }

        if (text.startsWith("BEEPING=")){
          String val = (text.substring(text.indexOf("=") + 1, text.length()));
          beepingOn = val.toInt();
          if (beepingOn){
            Serial.println("Beeping enabled");
          }else{
            Serial.println("Beeping disabled");
          }
        }

        if (text.startsWith("ALARM_TIME=")){
          String val = (text.substring(text.indexOf("=") + 1, text.indexOf(":")));
          alarmHour = val.toInt();
          val = (text.substring(text.indexOf(":") + 1, text.length()));
          alarmMin = val.toInt();      
          setAlarmTo(alarmHour, alarmMin);
          Serial.print("Alarm set to ");
          Serial.print(alarmHour);
          Serial.print(':');
          Serial.println(alarmMin);        
        }

        if (text.startsWith("ALARM_DAYS")){
          String val = (text.substring(text.indexOf("=") + 1, text.length()));

          Serial.println("Setting alarm days to " + val);

          alarmDays[0] = (val.indexOf("Su") > -1) ? true : false;   
          alarmDays[1] = (val.indexOf("Mo") > -1) ? true : false;
          alarmDays[2] = (val.indexOf("Tu") > -1) ? true : false;
          alarmDays[3] = (val.indexOf("We") > -1) ? true : false;
          alarmDays[4] = (val.indexOf("Th") > -1) ? true : false;
          alarmDays[5] = (val.indexOf("Fr") > -1) ? true : false;
          alarmDays[6] = (val.indexOf("Sa") > -1) ? true : false;
        }                               
      }

      break;

    case WStype_BIN:

      hexdump(payload, lenght);

      // echo data back to browser
      webSocket.sendBIN(num, payload, lenght);
      break;
  }

}

time_t getInternetTime() {
  
  long timestamp = 0;
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(timeDBHost, httpPort)) {
    Serial.println("connection failed");
    return 0;
  }

  Serial.print("Updating time: ");
  Serial.print(timeDBHost);
  Serial.println(timeDBPath);

  client.print(String("GET ") + timeDBPath + " HTTP/1.1\r\n" +
               "Host: " + timeDBHost + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(300);
  yield();

  // Read all server output and print do serial
  String json = "";
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (line.indexOf('{') > -1)
      json = line.substring(line.indexOf('{'), line.indexOf('}') + 1);
  }

  yield();

  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);

  yield();

  if (!root.success()) {
    Serial.print("ParseObject() failed");
  }else{
    timestamp = root["timestamp"];  
  }
  
  client.stop();

  Serial.print("Returning ");
  Serial.println(timestamp);

  return timestamp;
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void updateVariables(String text){

   //  CLAPPER=0&ALARM=1&DIMMER=169&SUNRISE=1&BEEPING=1&ALARM_TIME=15:29&ALARM_DAYS=SuMoTuWeThFrSa   
   
  String val = (text.substring(text.indexOf("CLAPPER=") + 8, text.indexOf("&ALARM=")));
  bool clapperOn = val.toInt();
  if (clapperOn){
    enableClapper();
    Serial.println("Clapper enabled");
  }else{
    disableClapper();
    Serial.println("Clapper disabled");
  }

  val = (text.substring(text.indexOf("&ALARM=") + 7, text.indexOf("&DIMMER=")));
  alarmOn = val.toInt();
  if (alarmOn){
    Serial.println("Alarm enabled");
  }else{
    Serial.println("Alarm disabled");
  }
   
  val = (text.substring(text.indexOf("&DIMMER=") + 8, text.indexOf("&SUNRISE=")));
  int dimmer = val.toInt();

  fade(dimmer,1000);
  lastDimmer = dimmer;
  Serial.print("Dimmer set to ");
  Serial.println(dimmer);
  
  val = (text.substring(text.indexOf("&SUNRISE=") + 9, text.indexOf("&BEEPING=")));
  sunriseOn = val.toInt();
  if (sunriseOn){
    Serial.println("Sunrise enabled");
  }else{
    Serial.println("Sunrise disabled");
  }

  val = (text.substring(text.indexOf("&BEEPING=") + 9, text.indexOf("&ALARM_TIME=")));
  beepingOn = val.toInt();
  if (beepingOn){
    Serial.println("Beeping enabled");
  }else{
    Serial.println("Beeping disabled");
  }

  val = (text.substring(text.indexOf("&ALARM_TIME=") + 12, text.indexOf("&ALARM_DAYS=")));
  alarmHour = val.toInt();
  val = (text.substring(text.indexOf(":") + 1, text.length()));
  alarmMin = val.toInt();      
  setAlarmTo(alarmHour, alarmMin);
  Serial.print("Alarm set to ");
  Serial.print(alarmHour);
  Serial.print(':');
  Serial.println(alarmMin);        

  val = (text.substring(text.indexOf("&ALARM_DAYS") + 1, text.length()));
  Serial.println("Setting alarm days to " + val);
  alarmDays[0] = (val.indexOf("Su") > -1) ? true : false;   
  alarmDays[1] = (val.indexOf("Mo") > -1) ? true : false;
  alarmDays[2] = (val.indexOf("Tu") > -1) ? true : false;
  alarmDays[3] = (val.indexOf("We") > -1) ? true : false;
  alarmDays[4] = (val.indexOf("Th") > -1) ? true : false;
  alarmDays[5] = (val.indexOf("Fr") > -1) ? true : false;
  alarmDays[6] = (val.indexOf("Sa") > -1) ? true : false;
}

void syncState(){
  
  String data = getStateString();

  // only try writing if different
  if (data.equals(lastSavedStateString))
    return;

  char buf[MAX_STATE_LENGTH];
  for (int i = 0; i < MAX_STATE_LENGTH; i++){
    buf[i] = 0;
  }
  
  data.toCharArray(buf, MAX_STATE_LENGTH);

  for (int i = 0; i < MAX_STATE_LENGTH; i++){
    EEPROM.put(STATE_EEPROM_ADDR + i, buf[i]);
  }  
  
  EEPROM.commit();

  lastSavedStateString = data;

  Serial.println("State saved to EEPROM.");
}

void loadState(){

  char buf[MAX_STATE_LENGTH];
  
  for (int i = 0; i < MAX_STATE_LENGTH; i++){
    buf[i] = 0;
  }

  for (int i = 0; i < MAX_STATE_LENGTH; i++){
    EEPROM.get(STATE_EEPROM_ADDR + i, buf[i]);
  }    

  String state(buf);
  
  Serial.print("EEPROM state loaded: ");
  Serial.println(state);  

  // update variables based on eeprom state
  updateVariables(state);  
}

void buttonISR(){
  
  int elapsed = millis() - lastButtonPush;
  
  // ignore if this is switch bounce
  if (buttonPushed || elapsed < BUTTON_HYSTERESIS)
    return;

  // do stuff here
  buttonPushed = true;

  lastButtonPush = millis();
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {

  // let power stabilize
  delay(300);
  
  Serial.begin(115200);

  initDimmer();
  initClapper();
  initBuzzer();

  pinMode(BUTTON, INPUT_PULLUP); 
  attachInterrupt(BUTTON, buttonISR, FALLING);

  // initialize random number generator
  randomSeed(analogRead(A0));

  Serial.println("Refreshing state from EEPROM...");
  // number of necessary bytes
  EEPROM.begin(MAX_STATE_LENGTH);  
  loadState();
  lastSavedStateString = getStateString();

  long start = millis();

  // Bring dimmer to last saved state before trying to connect
  while (millis() < start + 3000){
    loopDimmer();
    delay(10);    
  }
  
  WiFiManager wifiManager;

  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect("Lamp", "LampPassword");

  yield();

  while (WiFi.status() != WL_CONNECTED) {
    yield();
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  setSyncProvider(getInternetTime);
  setSyncInterval(TIME_SYNC_INTERVAL);

  alarmID = Alarm.alarmRepeat(12,00,00,alarmTime);
  sunriseAlarmID = Alarm.alarmRepeat(12,00,00,sunriseTime);

//  Alarm.timerRepeat(10,digitalClockDisplay);
  Alarm.timerRepeat(60, syncState);
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  if (!MDNS.begin("lamp")) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 81);  
}

void loop() {

  boolean twoClaps = isClapsEqual(2);

  int buttonPushes = 0;
  
  // if we detect 2 claps
  if (twoClaps || buttonPushed) {

    if (soundingAlarm){
      beepAck();
      // Stop sound alarm
      Alarm.free(soundAlarmID);      
      soundingAlarm = false;
      Serial.println("Alarm stopped.");
    }else{
      // if lights off...
      if (getDimmer() == 0){      
        if (lastDimmer != 0){
          // restore last user setting
          fade(lastDimmer, 1000);
        }else{
          // if user setting was 0, fade in to 100%
          fade(100, 1000);
          lastDimmer = 100;
        }
  
      }else{
        // if lights on...
        fade(0, 1000);
      }

      if (buttonPushed)
        buttonPushes++;

      if (buttonPushes == 5)
        easterEgg();
    }

    buttonPushed = false;
  }

  loopDimmer();
  webSocket.loop();
  Alarm.delay(10);
}
