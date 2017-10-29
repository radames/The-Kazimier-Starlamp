#include "Scheduler.h"
#include <Arduino.h>
#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

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
  _now_seconds = convertToSec(hour, min, sec) ;

  for (uint8_t i = 0; i < arr_len(_events) - 1 ; ++i) {
    _events[i].update(_now_seconds);
  }


}

void Scheduler::setStart(const char* time) {
  _n_startTime = convertToSec(time);
}

void Scheduler::setEnd(const char* time) {
  _n_endTime = convertToSec(time);
}

void Scheduler::setEvent(int id, const char* startTime, const char* endTime, const char* period, void (*callback)(int eventId)) {
  _events[id]._id = id;
  _events[id]._startTime = convertToSec(startTime);
  _events[id]._endTime = convertToSec(endTime);
  _events[id]._period = convertToSec(period);
  _events[id]._callback = callback;
  _events[id]._lastEventTime = 0;
}

bool SchedulerEvent::update(uint32_t now)
{
  bool doneEvent = false;
  if (now >= _startTime && now < _endTime) {
    //only runs  in this interval
    if ((now - _lastEventTime >= _period)) {
      _lastEventTime =  now;
      (*_callback)(_id);
    }

  }

  doneEvent = true;
  return doneEvent;
}


