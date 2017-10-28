#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <Arduino.h>
#define MAX_EVENTS 10

class Scheduler;
class Event
{
    friend class Scheduler;
  public:
    //Event();

    bool update(uint32_t now);

  protected:
    uint32_t _lastEventTime;
    uint32_t _period;
    uint32_t _startTime;
    uint32_t _endTime;
    int _count;
    int _repeatCount;
    void (*_callback)(uint32_t now);
};

class Scheduler
{
  public:
    Scheduler();

    int8_t setEvent(int id, const char* startTime, const char* endTime, void (*callback)(uint32_t ts), int repeatCount=-1);

    void update(uint8_t hour, uint8_t min, uint8_t sec);
    void setStart(const char* time);
    void setEnd(const char* time);
    void setNowCallback(uint32_t (*now)()) {_now = now;}


  protected:
    uint32_t      (*_now)();
    Event _events[MAX_EVENTS];
    static int _eventID;
    int _trackeventid;
    uint32_t n_seconds;
    uint32_t n_startTime;
    uint32_t n_endTime;
};



#endif /* SCHEDULER_H_ */

