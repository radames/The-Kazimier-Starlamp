#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <Arduino.h>

#define MAX_EVENTS (5)


enum TimeEvents {WAIT,CHECK_PERIOD, EVENT_START, WAIT_LENGTH, EVENT_END, WAIT_NEXT_PERIOD, PERIOD_END};

class Scheduler;
class SchedulerEvent
{
    friend class Scheduler;
  public:
    //SchedulerEvent();
    bool update(uint32_t now);
  private:
   TimeEvents _timeEvent;
  protected:
    int _id;
    bool _eventState;
    bool _newPeriod;
    uint32_t _nextEventTime;
    uint32_t _lastEventTime;
    uint32_t _lastPeriodTime;
    uint32_t _period;
    uint32_t _timeLength;
    uint32_t _startTime;
    uint32_t _endTime;
    void (*_callback)(int eventId, bool eventState);
};


class Scheduler
{
  public:
    Scheduler();

    void setEvent(int id, const char* startTime, const char* endTime, const char* period, const char* timeLength, void (*callback)(int eventId, bool eventState));

    bool update(uint8_t hour, uint8_t min, uint8_t sec);
    void setStart(const char* time);
    void setEnd(const char* time);


  protected:
    static int _eventID;
    int _trackeventid;
    uint32_t _now_seconds;
    uint32_t _n_startTime;
    uint32_t _n_endTime;

    SchedulerEvent  _events[MAX_EVENTS];
};



#endif /* SCHEDULER_H_ */

