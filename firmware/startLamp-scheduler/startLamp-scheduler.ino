#include "parameters.h"
#include "WifiPass.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h> //NTPClient by Arduino
#include <WiFiUdp.h>
#include <Wire.h>
#include <EEPROM.h>
#include <RTClib.h> //RTClib by Adafruit
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#include "Scheduler.h"
#include "AudioAnalysis.h"
#include "MP3Player.h"

SoftwareSerial mySoftwareSerial(14, 12, false, 256); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

Scheduler mScheduler;
AudioAnalysis mAudio;
MP3Player tracks[AUDIO_TRACKS];

RTC_DS3231 rtc;
WiFiUDP ntpUDP;

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, NTP_SERVER, GMT_TIME_ZONE * 3600 , 60000);

int timeUpdated = 0;
long lastPrintTime = 0;

enum EventState {EVENT1, EVENT2, AMBIENT};
EventState nState = AMBIENT;

void setup() {
  Serial.begin(9600);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  //DFPlayer Startup
  mySoftwareSerial.begin(9600);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  Serial.print("\n\n");
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  Serial.print("\n\n");

  Serial.println("Waiting to Start....");
  delay(5000);

  syncTime();

  mAudio.initAudioAnalisys();
  myDFPlayer.volume(MP3_VOLUME);  //Set volume value. From 0 to 30
  for (int i = 0; i < AUDIO_TRACKS; i++) {
    tracks[i].start(myDFPlayer);
  }
  mScheduler.setStart("16:00:00");
  mScheduler.setEnd("22:00:00");
  
  mScheduler.setEvent(EVENT1, "16:03:00", "16:10:00", "00:01:00", "00:00:10", schedulerCallBack);
  //mScheduler.setEvent(EVENT2, "03:00:20", "04:37:00", "00:00:20", schedulerCallBack);

}


void syncTime(void) {

  //Connect to Wifi
  //SSID and Password on WifiPass.h file
  Serial.println("Checking WIFI...");
  Serial.print("\n\n");

  WiFi.begin(ssid, password);
  delay(10000); //wait 10 secods for connection

  switch (WiFi.status()) {
    case WL_CONNECTED:
      {
        Serial.println("Wifi Connected...");
        Serial.println("Syncing NTP clock");
        timeClient.begin();
        timeClient.update();
        timeClient.update();
        delay(1000);
        Serial.print("Time before sync: ");
        logDateTime();

        long actualTime = timeClient.getEpochTime();
        Serial.print("Internet Epoch Time: ");
        Serial.print(actualTime);
        Serial.print("  ");
        Serial.println(timeClient.getFormattedTime());
        Serial.println();
        rtc.adjust(DateTime(actualTime));
        Serial.println("RTC Synced with NTP time");

        Serial.print("Time after sync: ");
        logDateTime();
      }
      break;

    case WL_NO_SSID_AVAIL:
      {
        Serial.println("No Wifi SSID detected...");
        Serial.println("Time not synced..");
      }
      break;
  }
  Serial.print("\n\n");

  //Turn Off WIFI
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

}

void loop () {
  DateTime now = rtc.now();
  mScheduler.update(now.hour(), now.minute(), now.second());


  if (millis() - lastPrintTime > 100) { //po
    lastPrintTime = millis();
    logDateTime();
  }

  switch (nState) {
    case AMBIENT:
      ambientIdle();
      break;
    case EVENT1:
        event1Func();
      break;
    case EVENT2:
        event2Func();
      break;
  }

  //audio();
}


void ambientIdle(void) {
  //play track 1 on loop
  if (!tracks[1].isPlaying()) { 
    resetTracksState();
    tracks[1].loop();
  }
  ledOSC();
}


void ledOSC() {
  //sine wave led pattern
  float t = (float)millis() / LED_OSC_PERIOD;
  int pValue = 0.5 * LED_OSC_MAX_AMP * (1 + sin(2.0 * PI * t));
  analogWrite(LED_PIN, pValue);
}

void schedulerCallBack(int eventId, bool eventState) {
  
  Serial.print("EVENT--->  ");
  Serial.print(eventId);
  Serial.print("  ");
  if(eventState){
    Serial.print("STARTED");
    nState = (EventState)eventId;
  }else{
    Serial.print("ENDED");
    nState = AMBIENT;
  }
  Serial.println("  ");

}

void event1Func() {
  //1 min
  //play track 2 (Bell Sound 1)
  if (!tracks[2].isPlaying()) {
    resetTracksState();
    tracks[2].play();  
  }
  int input = analogRead(MIC_PIN);
  unsigned int out = mAudio.analysis(input);
  analogWrite(LED_PIN, out);
}

void event2Func() {
  //2min
  //play track 3 track 2 (Bell Sound 2)
  if (!tracks[3].isPlaying()) {
    resetTracksState();
    tracks[3].play();  
  }
  int input = analogRead(MIC_PIN);
  unsigned int out = mAudio.analysis(input);
  analogWrite(LED_PIN, out);
}

void logDateTime(void) {
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC); Serial.print('/');
  Serial.print(now.month(), DEC); Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print("  ");
  Serial.print(now.hour(), DEC); Serial.print(':');
  Serial.print(now.minute(), DEC); Serial.print(':');
  Serial.print(now.second(), DEC); Serial.println();
}

void resetTracksState(){
  for (int i = 0; i < AUDIO_TRACKS; i++) {
    tracks[i].stop();
  }
}
