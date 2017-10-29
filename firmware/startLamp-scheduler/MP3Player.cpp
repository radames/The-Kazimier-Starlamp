#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "MP3Player.h"

int MP3Player::_id = 1;
MP3Player::MP3Player()
{
  _trackid = _id++;
  _value = 0;
  _isPlaying = false;
}
void MP3Player::start(DFRobotDFPlayerMini &DFPlayer) {
  _DFPlayer = &DFPlayer;
}
void MP3Player::play() {
  _DFPlayer->play(_trackid);
   _isPlaying = true;
}
void MP3Player::loop() {
  _DFPlayer->play(_trackid);
  _isPlaying = true;
}
void MP3Player::stop() {
  _DFPlayer->stop();
  _isPlaying = false;
}
bool MP3Player::isPlaying(){
  return _isPlaying;
}

void MP3Player::update(int value)
{
  _value = value;
  if (value >= 254) {
    _DFPlayer->play(_trackid);

  } else if (value >= 10 && value <= 11) {
    _DFPlayer->stop();

  }
}
