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

bool Scheduler::update(uint8_t hour, uint8_t min, uint8_t sec) {
  //store actual time in seconds;
  _now_seconds = convertToSec(hour, min, sec) ;

  if (_now_seconds >= _n_startTime && _now_seconds <= _n_endTime) {
    //only runs the Scheduler within the start end time
    for (uint8_t i = 0; i < arr_len(_events) - 1 ; ++i) {
      _events[i].update(_now_seconds);
    }
    return true; //scheduler running
  } else {
    return false; //scheduler not running
  }
}

void Scheduler::setStart(const char* time) {
  _n_startTime = convertToSec(time);
}

void Scheduler::setEnd(const char* time) {
  _n_endTime = convertToSec(time);
}

void Scheduler::setEvent(int id, const char* startTime, const char* endTime, const char* period, const char* timeLength, void (*callback)(int eventId, bool eventState)) {
  _events[id]._id = id;
  _events[id]._startTime = convertToSec(startTime);
  _events[id]._endTime = convertToSec(endTime);
  _events[id]._period = convertToSec(period);
  _events[id]._timeLength = convertToSec(timeLength);
  _events[id]._callback = callback;
  _events[id]._lastEventTime = 0;
  _events[id]._lastPeriodTime = 0;
  _events[id]._timeEvent = WAIT_NEXT_PERIOD;
}

bool SchedulerEvent::update(uint32_t now)
{
  if (now >= _startTime && now <= _endTime) {
    //only runs in the starend window
    switch (_timeEvent) {
      case CHECK_PERIOD:
        _lastPeriodTime = 2 * now - (_startTime + _period);
        _timeEvent = EVENT_START;
        break;
      case EVENT_START:
        (*_callback)(_id, true); //event start
        _timeEvent = WAIT_LENGTH;
        _lastEventTime = now;
        break;
      case WAIT_LENGTH:
        if (now - _lastEventTime >= _timeLength) {
          _timeEvent = EVENT_END;
        };
        break;
      case EVENT_END:
        (*_callback)(_id, false); //event start
        _timeEvent = WAIT_NEXT_PERIOD;
        break;
      case WAIT_NEXT_PERIOD:
        if (now - _lastPeriodTime >= _period) {
          _lastPeriodTime = now;
          _timeEvent = EVENT_START;
        };
        break;
    }

  } else {
    return false;
  }
}

//  if (now >= _startTime && now <= _endTime) {
//    //only runs  in this interval
//
//    if (_newPeriod) {
//      //trigger start and end event
//      if (_eventState == false) {
//        _eventState = true; //event has started
//        (*_callback)(_id, _eventState);
//        _lastEventTime = now;
//      } else {
//        //check for timeLength
//        if (now - _lastEventTime >= _timeLength) {
//          _eventState = false; //event has endeded
//          (*_callback)(_id, _eventState);
//          _newPeriod = true;
//        }
//      }
//    }
//    if (now - _lastPeriodTime > _period) {
//      //take care of period events
//      _lastPeriodTime = now;
//      _newPeriod = true;
//    }
//
//  }





