#include "WifiPass.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h> //NTPClient by Arduino
#include <WiFiUdp.h>
#include <Wire.h>
#include <EEPROM.h>
#include <RTClib.h> //RTClib by Adafruit

#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(14, 12, false, 256); // RX, TX
DFRobotDFPlayerMini myDFPlayer;


//closest NTP Server
#define NTP_SERVER "0.us.pool.ntp.org"

//GMT Time Zone with sign
#define GMT_TIME_ZONE -7

//Force RTC update and store on EEPROM
//change this to a random number between 0-255 to force time update
#define FORCE_RTC_UPDATE 1

//MP3 Volume 0-30
#define MP3_VOLUME 25

//Audio analysis MIN-MAX 0-1024

#define PWM_MIN 0
#define PWM_MAX 1024

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

RTC_DS3231 rtc;
WiFiUDP ntpUDP;

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, NTP_SERVER, GMT_TIME_ZONE * 3600 , 60000);

int timeUpdated = 0;
long lastPrintTime = 0;

void setup() {
  Serial.begin(9600);

  //RTC and EEPROM Startup
  EEPROM.begin(4);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }


  //DFPlayer Startup
  mySoftwareSerial.begin(9600);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));

  Serial.println("Waiting to Start....");
  delay(5000);

  //check eeprom and sync RTCClock
  checkSyncedTime();

  initAudioAnalisys();


  myDFPlayer.volume(MP3_VOLUME);  //Set volume value. From 0 to 30
  myDFPlayer.loop(1);  //Play the first mp3
}

void checkSyncedTime(void) {
  //Read the EEPROM to check if time has been synced

  byte addvalue = EEPROM.read(timeUpdated);
  Serial.print("EEPROM: ");
  Serial.print(addvalue);
  Serial.print(" == ");
  Serial.print(FORCE_RTC_UPDATE);
  Serial.println(" ?");
  if (addvalue != FORCE_RTC_UPDATE) {
    //if(true == false){
    //time hasn' it been setup
    Serial.println("Forcing Time Update");
    syncTime();
    Serial.println("Updating EEPROM..");
    EEPROM.write(timeUpdated, FORCE_RTC_UPDATE);
    EEPROM.commit();

  } else {
    Serial.println("Time has been updated before...EEPROM CHECKED");
    Serial.print("EEPROM: ");
    Serial.print(addvalue);
    Serial.print(" = ");
    Serial.print(FORCE_RTC_UPDATE);
    Serial.println("!");
  }

}
void syncTime(void) {

  //Connect to Wifi
  //SSID and Password on WifiPass.h file
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
  timeClient.update();

  long actualTime = timeClient.getEpochTime();
  Serial.print("Internet Epoch Time: ");
  Serial.println(actualTime);
  rtc.adjust(DateTime(actualTime));



  //Turn Off WIFI after update
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();

}


#define N_PIXELS  500  // Number of LEDs
#define MIC_PIN   A0  // Microphone is attached to this analog pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     10  // Noise/hum/interference in mic signal
#define SAMPLES   100  // Length of buffer for dynamic level adjustment
#define TOP       (N_PIXELS + 2) // Allow dot to go slightly off scale
#define PEAK_FALL 10  // Rate of peak falling dot
int led = D4;
byte
peak      = 0,      // Used for falling dot
dotCount  = 0,      // Frame counter for delaying dot-falling speed
volCount  = 0;      // Frame counter for storing past volume data
int
vol[SAMPLES],       // Collection of prior volume samples
lvl       = 10,      // Current "dampened" audio level
minLvlAvg = 0,      // For dynamic adjustment of graph low & high
maxLvlAvg = 512;


void initAudioAnalisys(void){

  memset(vol, 0, sizeof(vol));
  for(int i=0; i<1024; i++){
    analogWrite(led, i);
    delay(10);
  }

}


void audio()
{
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int n, height;
  n   = analogRead(MIC_PIN); // Raw reading from mic
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);     // Remove noise/hum
  lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);

  if (height < 0L) height = 0; // Clip output
  else if (height > TOP) height = TOP;
  if (height > peak) peak = height; // Keep 'peak' dot at top

  if (peak > 0 && peak <= N_PIXELS - 1)
  {
    // colour peak dot
  }
  if (++dotCount >= PEAK_FALL) { //fall rate

    if (peak > 0) peak--;
    dotCount = 0;
  }
  vol[volCount] = n;   // Save sample for dynamic leveling
  if (++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter
  // Get volume range of prior frames
  minLvl = maxLvl = vol[0];
  for (i = 1; i < SAMPLES; i++) {
    if (vol[i] < minLvl) minLvl = vol[i];
    else if (vol[i] > maxLvl) maxLvl = vol[i];
  }
  if ((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

 // Serial.println(4 * peak);
  int pwmV = map(4 * peak, 250, 700, 0, 1024);
  pwmV = constrain(pwmV, PWM_MIN, PWM_MAX);
  analogWrite(led, pwmV);
}


void loop () {

  if (millis() - lastPrintTime > 10000) {
    lastPrintTime = millis();
    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.println();
  }

  audio();
}
