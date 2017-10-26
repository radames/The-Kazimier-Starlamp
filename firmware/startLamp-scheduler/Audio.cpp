#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "Audio.h"

int Audio::_id = 1;
Audio::Audio()
{
  _trackid = _id++;
  _value = 0;
  _isPlaying = false;
}
void Audio::start(DFRobotDFPlayerMini &DFPlayer){
  _DFPlayer = &DFPlayer;
}
void Audio::update(int value)
{
  _value = value;
  if(value >= 254){
    _DFPlayer->play(_trackid);
    
  }else if(value >= 10 && value <= 11){
    _DFPlayer->stop();
    
  }
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


void Audio::initAudioAnalisys(void) {

  memset(vol, 0, sizeof(vol));
  for (int i = 0; i < 1024; i++) {
    analogWrite(led, i);
    delay(10);
  }

}


void Audio::audio(void)
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
