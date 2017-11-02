#include "parameters.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#include "AudioAnalysis.h"
#include "MP3Player.h"

SoftwareSerial mySoftwareSerial(14, 12, false, 256); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

AudioAnalysis mAudio;
MP3Player tracks[AUDIO_TRACKS];


enum EventState {EVENT1, EVENT2, AMBIENT};
EventState nState = EVENT2;
EventState lastState = EVENT2;

long lastMillis = 0;
long lastPrintTime = 0;

void setup() {
  Serial.begin(9600);
  Serial.print("\n\n\n\n");

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
  delay(1000);


  mAudio.initAudioAnalisys();
  myDFPlayer.volume(MP3_VOLUME);  //Set volume value. From 0 to 30

  for (int i = 0; i < AUDIO_TRACKS; i++) {
    Serial.print("MP3-ID: ");
    Serial.println(tracks[i].start(myDFPlayer));
#ifdef DEBUG_MODE
    tracks[i].play();
    delay(5000);
#endif
  }
  resetTracksState();

  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  
  lastMillis = millis();
  lastPrintTime = millis();
}



void loop () {

#ifdef DEBUG_MODE
  if (millis() - lastPrintTime > 1000) { //po
    lastPrintTime = millis();
    Serial.println((millis() - lastMillis)/1000);
  }
#endif /* debug mode print actual time */


  switch (nState) {
    case EVENT1:
      event1Func();
      if (millis() - lastMillis > E1_LENGTH*1000) {
        lastMillis = millis();
        lastState = EVENT1;
        nState = AMBIENT;
      }
      break;
    case EVENT2:
      event2Func();
      if (millis() - lastMillis > E2_LENGTH*1000) {
        lastMillis = millis();
        lastState = EVENT2;
        nState = AMBIENT;
      }
      break;
    case AMBIENT:
      ambientIdle();
      if (millis() - lastMillis > AMBIENT_LENGTH*1000) {
        lastMillis = millis();
        nState = (lastState == EVENT1)? EVENT2: EVENT1;
      }
      break;
  }

}


void ambientIdle(void) {
  //play track 1 on loop
  if (!tracks[0].isPlaying()) {
    resetTracksState();
    tracks[0].loop();
  }
  ledOSC();
}


void ledOSC() {
  //sine wave led pattern
  float t = (float)millis() / LED_OSC_PERIOD;
  int pValue = 0.5 * LED_OSC_MAX_AMP * (1 + sin(2.0 * PI * t));
  analogWrite(LED_PIN, pValue);
}

void event1Func() {
  //1 min
  //play track 2 (Bell Sound 1)
  if (!tracks[1].isPlaying()) {
    resetTracksState();
    tracks[1].play();
    mAudio.resetParams();
  }
  int input = analogRead(MIC_PIN);
  unsigned int out = mAudio.analysis(input);
  analogWrite(LED_PIN, out);
}

void event2Func() {
  //2min
  //play track 3 track 2 (Bell Sound 2)
  if (!tracks[2].isPlaying()) {
    resetTracksState();
    tracks[2].play();
    mAudio.resetParams();
  }
  int input = analogRead(MIC_PIN);
  unsigned int out = mAudio.analysis(input);
  analogWrite(LED_PIN, out);
}

void resetTracksState() {
  for (int i = 0; i < AUDIO_TRACKS; i++) {
    tracks[i].stop();
  }
}

