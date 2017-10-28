#ifndef MP3Player_h
#define MP3Player_h

#include "MP3Player.h"
#include "parameters.h"

class MP3Player
{
  public:
    MP3Player();
    void update(int value);
    void start(DFRobotDFPlayerMini &DFPlayer);
    void play();

  private:
    static int _id;
    int _trackid;
    DFRobotDFPlayerMini* _DFPlayer;
    int _value;
    bool _isPlaying;
};

#endif
