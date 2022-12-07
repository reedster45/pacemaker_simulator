

#include "pacemaker.h"



// declare variables
int coid, rcvid;
name_attach_t *attach;
pulse_t pace;

struct sigevent atrium_event, ventricle_event;
timer_t atrium_timerid, ventricle_timerid;
struct itimerspec atrium_it, ventricle_it;



// initialize heartbeat timers
void init_heartbeat() {
	// setup right atrium timer
//	SIGEV_PULSE_INIT(&atrium_event, coid, SIGEV_PULSE_PRIO_INHERIT, RIGHT_ATRIUM_EVENT, 0);
//	timer_create(CLOCK_REALTIME, &atrium_event, &atrium_timerid);
//
//	atrium_it.it_value.tv_sec = 5;
//	atrium_it.it_value.tv_nsec = 0;
//	atrium_it.it_interval.tv_sec = 1;
//	atrium_it.it_interval.tv_nsec = 500000000;
//
//	timer_settime(atrium_timerid, 0, &atrium_it, NULL);


	// cleanup
	timer_delete(ventricle_timerid);

	// setup right ventricle timer
	timer_create(CLOCK_REALTIME, &ventricle_event, &ventricle_timerid);

	ventricle_it.it_value.tv_sec = 1;
	ventricle_it.it_value.tv_nsec = 0;
	ventricle_it.it_interval.tv_sec = 1;
	ventricle_it.it_interval.tv_nsec = 200000000;

	timer_settime(ventricle_timerid, 0, &ventricle_it, NULL);
}


// receive pulse/msg from the pacemaker
void heart_pace() {
	// receive pulse from pacemaker
	rcvid = MsgReceive(attach->chid, &pace, sizeof(pace), NULL);

	if (0 == rcvid) {
		//received a pulse
		switch (pace.pulse.code) {
		case _PULSE_CODE_DISCONNECT:
			cout << "pacemaker ran out of battery." << endl;
			break;

		case PACE_HEART_EVENT:
			init_heartbeat();
			break;
		}

	 } else {
		 cout << "something is wrong!" << endl;
	 }
}


int main() {
	// create a channel to pace heart
	attach = name_attach(NULL, "heart", 0);

	//establish a connection to the pacemaker
	coid = name_open("pacemaker", 0);

	// notify pacemaker heart channel is open
	MsgSend(coid, &coid, sizeof(coid), NULL, sizeof(NULL));

	// initialize heart events and heartbeat
	SIGEV_PULSE_INIT(&ventricle_event, coid, SIGEV_PULSE_PRIO_INHERIT, RIGHT_VENTRICLE_EVENT, 0);
	init_heartbeat();

	cout << "heart is beating..." << endl;

	// receive pace pulse from pacemaker if heartbeat is irregular
	while(1) {
		heart_pace();
	}

    // Close a server connection that was opened by name_open()
	name_close(coid);

	return 0;
}







