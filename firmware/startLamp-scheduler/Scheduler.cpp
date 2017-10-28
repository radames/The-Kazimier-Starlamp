#include "Scheduler.h"
#include <Arduino.h>




static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}
static uint32_t convertToSec(const char* time) {
  uint8_t hh = conv2d(time);
  uint8_t mm = conv2d(time + 3);
  uint8_t ss = conv2d(time + 6);
  return hh * 60 * 60 + mm * 60 + ss;
}
static uint32_t convertToSec(uint8_t hour, uint8_t min, uint8_t sec) {
  return hour * 60 * 60 + min * 60 + sec;
}

int Scheduler::_eventID = 0;
Scheduler::Scheduler() {
    _trackeventid = _eventID++;

}

void Scheduler::update(uint8_t hour, uint8_t min, uint8_t sec) {
  //store actual time in seconds;
  n_seconds = convertToSec(hour, min, sec) ;
}

void Scheduler::setStart(const char* time) {
  n_startTime = convertToSec(time);
}

void Scheduler::setEnd(const char* time) {
  n_endTime = convertToSec(time);
}


