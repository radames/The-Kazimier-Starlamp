#ifndef Audio_h
#define Audio_h

#include "Audio.h"
#include "parameters.h"

class Audio
{
  public:
    Audio();
    void update(int value);
    void start(DFRobotDFPlayerMini &DFPlayer);
    void audio(void);
    void initAudioAnalisys(void);
  private:
    static int _id;
    int _trackid;
    DFRobotDFPlayerMini* _DFPlayer;
    int _value;
    bool _isPlaying;
};

#endif
