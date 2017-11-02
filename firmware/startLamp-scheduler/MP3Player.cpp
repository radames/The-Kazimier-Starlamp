#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include "MP3Player.h"

int MP3Player::_id = 0;
MP3Player::MP3Player()
{
  _trackid = ++_id;
  _value = 0;
  _isPlaying = false;
}
int MP3Player::start(DFRobotDFPlayerMini &DFPlayer) {
  _DFPlayer = &DFPlayer;
  return _trackid;
}
void MP3Player::play() {
  _DFPlayer->play(_trackid);
  if (checkError()) {
    //check for error
#ifdef DEBUG_MODE
    Serial.print("ERROR DFPLAYER --------> PLAYING TRACK -----> ");
    Serial.println(_trackid);
#endif
    //keep _isPlaying false until next interaction
    _isPlaying = false;
  } else {
#ifdef DEBUG_MODE
    Serial.print("SUCCESS ---------------------> PLAYING TRACK -----> ");
    Serial.println(_trackid);
#endif
    _isPlaying = true;
  }
}
void MP3Player::loop() {
  _DFPlayer->loop(_trackid);
  if (checkError()) {
    //check for error
#ifdef DEBUG_MODE
    Serial.print("ERROR DFPLAYER --------> LOOPING TRACK -----> ");
    Serial.println(_trackid);
#endif
    //keep _isPlaying false until next interaction
    _isPlaying = false;
  } else {
#ifdef DEBUG_MODE
    Serial.print("SUCCESS ---------------------> LOOPING TRACK -----> ");
    Serial.println(_trackid);
#endif
    _isPlaying = true;
  }
}
void MP3Player::stop() {
  _DFPlayer->stop();
  if (checkError()) {
    //check for error
#ifdef DEBUG_MODE
    Serial.print("ERROR DFPLAYER --------> STOPING TRACK -----> ");
    Serial.println(_trackid);
#endif
    //keep _isPlaying false until next interaction
    _isPlaying = true;
  } else {
#ifdef DEBUG_MODE
    Serial.print("SUCCESS ---------------------> STOPING TRACK -----> ");
    Serial.println(_trackid);
#endif
    _isPlaying = false;
  }
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
bool MP3Player::checkError() {
  delay(100);
  if (_DFPlayer->available()) {
    if (_DFPlayer->readType() == DFPlayerError) {
      return true;
    }
  }
  return false; //return false if there is no data from the mp3 module
}
