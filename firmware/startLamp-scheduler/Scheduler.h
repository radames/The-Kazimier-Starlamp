#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <Arduino.h>

class Scheduler
{
  public:
    Scheduler();
    void update(uint8_t hour, uint8_t min, uint8_t sec);
    void setStart(const char* time);
    void setEnd(const char* time);

  protected:
    static int _eventID;
    int _trackid;
    uint32_t n_seconds;
    uint32_t n_startTime;
    uint32_t n_endTime;
    _trackeventid
};



#endif /* SCHEDULER_H_ */

