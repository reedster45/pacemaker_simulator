
#include <sys/time.h>
#include "pacemaker.h"

// declare variables
const int LRI=1000, VRP=250;
int pace_coid, sense_coid, rcvid;
unsigned long start_time;
name_attach_t *attach;
pulse_t heartbeat;

struct sigevent LRI_event, VRP_event;
timer_t LRI_timerid, VRP_timerid;
struct itimerspec LRI_it, VRP_it;

bool vSense = false, vPace = false, LRI_timeout=false, VRP_timeout=false;


// initialize vrp timer
void init_VRP() {
	// cleanup
	timer_delete(VRP_timerid);

	// setup VRP timer
	timer_create(CLOCK_REALTIME, &VRP_event, &VRP_timerid);

	VRP_it.it_value.tv_sec = 0;
	VRP_it.it_value.tv_nsec = VRP * 1000000;
	VRP_it.it_interval.tv_sec = 0;
	VRP_it.it_interval.tv_nsec = 0;

	timer_settime(VRP_timerid, 0, &VRP_it, NULL);
}


// initialize timers for timing cycles
void init_timecycle() {
	// cleanup
	timer_delete(LRI_timerid);
	timer_delete(VRP_timerid);

	// setup LRI timer
	timer_create(CLOCK_REALTIME, &LRI_event, &LRI_timerid);

	LRI_it.it_value.tv_sec = LRI / 1000;
	LRI_it.it_value.tv_nsec = 0;
	LRI_it.it_interval.tv_sec = LRI / 1000;
	LRI_it.it_interval.tv_nsec = 0;

	timer_settime(LRI_timerid, 0, &LRI_it, NULL);

	// setup VRP timer
	timer_create(CLOCK_REALTIME, &VRP_event, &VRP_timerid);

	VRP_it.it_value.tv_sec = 0;
	VRP_it.it_value.tv_nsec = VRP * 1000000;
	VRP_it.it_interval.tv_sec = 0;
	VRP_it.it_interval.tv_nsec = 0;

	timer_settime(VRP_timerid, 0, &VRP_it, NULL);
}


// handler for a ventricle beat
void handle_ventricle_event() {
	if (LRI_timeout == false) { // within LRI
		vSense = true;
		init_timecycle();
	}
	init_VRP();
	LRI_timeout = false;
	VRP_timeout = false;
}

//From: https://stackoverflow.com/questions/10192903/time-in-milliseconds-in-c#answer-44896326
long long curTime(void) {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000) - start_time * 1000;
}

// handler for LRI timeout
void handle_LRI_timeout() {
	if (vSense == false && VRP_timeout) { // havent sensed ventricle beat yet
		LRI_timeout = true;
		vPace = true; // pace heart to have ventricle beat
	}
	vSense = false;
}

// send pulse to heart if heart missed a beat
void pace_heart() {
	if (vPace) {
		cout << "pace beat" << endl;
		MsgSendPulse(pace_coid, -1, PACE_HEART_EVENT, 0);
		vPace = false;
	}
}


// receive pulse/msg from the heart
void heart_sense() {
	// receive pulse from heart
	rcvid = MsgReceive(attach->chid, &heartbeat, sizeof(heartbeat), NULL);

	pace_heart();

	if (rcvid == 0) {
		//received a pulse
		switch (heartbeat.pulse.code) {
		case _PULSE_CODE_DISCONNECT:
			cout << "heart is dead. t=" << curTime() << "ms"<< endl;
			break;

		case RIGHT_VENTRICLE_EVENT:
			handle_ventricle_event();
			cout << "ventricle beat t=" << curTime() << "ms"<< endl;
			break;

		case LRI_TIMEOUT:
			handle_LRI_timeout();
			cout << "LRI timeout t=" << curTime() << "ms"<< endl;
			break;

		case VRP_TIMEOUT:
			VRP_timeout = true;
			cout << "VRP timeout t=" << curTime() << "ms"<< endl;
			break;
		}

	 } else {
		//establish a connection to the heart
		start_time = time(NULL);
		pace_coid = name_open("heart", 0);
		MsgReply(rcvid, 0, NULL, sizeof(NULL));
		cout << "connected to heart." << endl;
	 }
	vSense = false;
}


int main() {
	// create a channel to sense heart
	attach = name_attach(NULL, "pacemaker", 0);

	// connect to sense heart channel
	sense_coid = name_open("pacemaker", 0);

	// initialize events
	SIGEV_PULSE_INIT(&LRI_event, sense_coid, SIGEV_PULSE_PRIO_INHERIT, LRI_TIMEOUT, 0);
	SIGEV_PULSE_INIT(&VRP_event, sense_coid, SIGEV_PULSE_PRIO_INHERIT, VRP_TIMEOUT, 0);

	// continuously monitor the heart
	while(1) {
		heart_sense();
	}

	return 0;
}



