#include "timing-cycle.h"
#include <cmath>

TimingCycle::TimingCycle(string name, int coid, int pulse_code){
	SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT, pulse_code, 0);
	created = false;
}

//delay (in ms), period (in ms)
void TimingCycle::start(int delay, int period){
	timer_create(CLOCK_REALTIME, &event, &timer_id);
	created = true;
	it.it_value.tv_sec = (int) (delay / 1000);
	it.it_value.tv_nsec = (delay % 1000) * 1000000 + 1;
	it.it_interval.tv_sec = (int) (period / 1000);
	it.it_interval.tv_nsec = (period % 1000) * 1000000;
	timer_settime(timer_id, 0, &it, NULL);
}

void TimingCycle::stop(){
	if (!created)return;
	timer_delete(timer_id);
}
