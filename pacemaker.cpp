#include "pacemaker.h"

// declare variables
const int LRI=1000, VRP=250;
int pace_coid, sense_coid, rcvid;
name_attach_t *attach;
pulse_t heartbeat;

struct sigevent LRI_event, VRP_event;
timer_t LRI_timerid, VRP_timerid;
struct itimerspec LRI_it, VRP_it;

TimingCycle *LRICycle;
TimingCycle *VRPCycle;

bool vSense = false, vPace = false, LRI_timeout=false, VRP_timeout;


// initialize timers for timing cycles
void init_timecycle() {
	// cleanup
	LRICycle->stop();
	//VRPCycle->stop();

	// setup time cycles
	LRICycle->start(0, LRI);
	VRPCycle->start(0, VRP);
}

// handler for a ventricle beat
void handle_ventricle_event() {
	// if during VRP period then we ignore ventricular  events
	if (!VRP_timeout) return;
	if (LRI_timeout == false) { // within LRI
		vSense = true;
		init_timecycle();
	}
	LRI_timeout = false;
}

// handler for LRI timeout
void handle_LRI_timeout() {
	if (vSense == false) { // havent sensed ventricle beat yet
		LRI_timeout = true;
		vPace = true; // pace heart to have ventricle beat
	}
	vSense = false;
}

// handler for VRP timeout
void handle_VRP_timeout(){
	VRP_timeout = true;
}

// send pulse to heart if heart missed a beat
void pace_heart() {
	if (vPace) {
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
			cout << "heart is dead." << endl;
			break;

		case RIGHT_ATRIUM_EVENT:
			cout << "atrium beat" << endl;
			break;

		case RIGHT_VENTRICLE_EVENT:
			handle_ventricle_event();
			cout << "ventricle beat" << endl;
			break;

		case LRI_TIMEOUT:
			handle_LRI_timeout();
			cout << "LRI timeout" << endl;
			break;

		case VRP_TIMEOUT:
			handle_VRP_timeout();
			cout << "VRP timeout" << endl;
			break;
		}

	 } else {
		//establish a connection to the heart
		pace_coid = name_open("heart", 0);
		MsgReply(rcvid, 0, NULL, sizeof(NULL));
		cout << "connected to heart." << endl;
	 }
}


int main() {
	// create a channel to sense heart
	attach = name_attach(NULL, "pacemaker", 0);

	// connect to sense heart channel
	sense_coid = name_open("pacemaker", 0);

	//Create our timing cycles
	LRICycle = new TimingCycle("LRI", sense_coid, LRI_TIMEOUT);
	VRPCycle = new TimingCycle("VRP", sense_coid, VRP_TIMEOUT);

	// continuously monitor the heart
	while(1) {
		heart_sense();
	}

	return 0;
}



