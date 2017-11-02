#ifndef AudioAnalysis_h
#define AudioAnalysis_h

#include "AudioAnalysis.h"
#include "parameters.h"

class AudioAnalysis
{
  public:
    AudioAnalysis();
    void initAudioAnalisys(void);
    unsigned int analysis(int input);
    void resetParams(void);
  private:      
    byte
    _peak      = 0,      // Used for falling dot
    _dotCount  = 0,      // Frame counter for delaying dot-falling speed
    _volCount  = 0;      // Frame counter for storing past volume data
    
    int
    _vol[SAMPLES],       // Collection of prior volume samples
    _lvl       = 10,      // Current "dampened" audio level
    _minLvlAvg = 0,      // For dynamic adjustment of graph low & high
    _maxLvlAvg = 512;
};

#endif
