#include "Arduino.h"
#include "AudioAnalysis.h"

AudioAnalysis::AudioAnalysis()
{
  //initAudioAnalisys();
}
void AudioAnalysis::initAudioAnalisys(void) {
  memset(_vol, 0, sizeof(_vol));
}
void AudioAnalysis::resetParams(void) {
  _peak      = 0;
  _dotCount  = 0;
  _volCount  = 0;
  _lvl       = 10;
  _minLvlAvg = 0;
  _maxLvlAvg = 512;
  for (int i = 0; i < SAMPLES; i++) {
    _vol[i] = 0;
  }
}
unsigned int AudioAnalysis::analysis(int input)
{
  uint8_t  i;
  uint16_t minLvl, maxLvl;
  int n, height;
  n   = input; // Raw reading from mic
  n   = abs(n - 512 - DC_OFFSET); // Center on zero
  n   = (n <= NOISE) ? 0 : (n - NOISE);     // Remove noise/hum
  _lvl = ((_lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)

  // Calculate bar height based on dynamic min/max levels (fixed point):
  height = TOP * (_lvl - _minLvlAvg) / (long)(_maxLvlAvg - _minLvlAvg);

  if (height < 0L) height = 0; // Clip output
  else if (height > TOP) height = TOP;
  if (height > _peak) _peak = height; // Keep 'peak' dot at top

  if (_peak > 0 && _peak <= N_PIXELS - 1)
  {
    // colour peak dot
  }
  if (++_dotCount >= PEAK_FALL) { //fall rate

    if (_peak > 0) _peak--;
    _dotCount = 0;
  }
  _vol[_volCount] = n;   // Save sample for dynamic leveling
  if (++_volCount >= SAMPLES) _volCount = 0; // Advance/rollover sample counter
  // Get volume range of prior frames
  minLvl = maxLvl = _vol[0];
  for (i = 1; i < SAMPLES; i++) {
    if (_vol[i] < minLvl) minLvl = _vol[i];
    else if (_vol[i] > maxLvl) maxLvl = _vol[i];
  }
  if ((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
  _minLvlAvg = (_minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
  _maxLvlAvg = (_maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)

  // Serial.println(4 * peak);
  unsigned int output = map(4 * _peak, 250, 700, OUT_MIN, OUT_MAX);
  output = constrain(output, OUT_MIN, OUT_MAX);
  return output;
}
