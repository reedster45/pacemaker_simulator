#ifndef TIMING_CYCLE_H_
#define TIMING_CYCLE_H_

#include "qnx.h"

class TimingCycle{
public:
	TimingCycle(string name, int coid, int pulse_code);
	void start(int delay, int period);
	void stop();
private:
	timer_t timer_id;
	struct itimerspec it;
	struct sigevent event;
	bool created;
};



#endif /* TIMING_CYCLE_H_ */
