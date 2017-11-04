#ifndef MP3Player_h
#define MP3Player_h

#include "MP3Player.h"
#include "parameters.h"

class MP3Player
{
  public:
    MP3Player();
    void update(int value);
    int start(DFRobotDFPlayerMini &DFPlayer);
    bool isPlaying();
    void loop();
    void play();
    void stop();
    void resetState();
   
  private:
    static int _id;
    int _trackid;
    DFRobotDFPlayerMini* _DFPlayer;
    bool checkError();
    int _value;
    bool _isPlaying;
};

#endif
