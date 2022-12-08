

#include "pacemaker.h"



// declare variables
int coid, rcvid, mode;
name_attach_t *attach;
pulse_t pace;

// 1-running,  2-normal,  3-bradycardia
int mode_bpm[3] = {100, 60, 50};
float bpm;

struct sigevent ventricle_event;
timer_t ventricle_timerid;
struct itimerspec ventricle_it;



// initialize heartbeat timers
void init_heartbeat() {
	float sec_value = 60/bpm;
	float nsec_value = 0;
	float sec_interval = trunc(60/bpm);
	float nsec_interval = ((60/bpm) - sec_interval) * 1000 * 1000000;

	if (sec_value > 1) {
		sec_value = 1;
	} else {
		sec_value = 0;
		nsec_value = 000000001;
	}

	// cleanup
	timer_delete(ventricle_timerid);

	// setup right ventricle timer
	timer_create(CLOCK_REALTIME, &ventricle_event, &ventricle_timerid);

	ventricle_it.it_value.tv_sec = sec_value;
	ventricle_it.it_value.tv_nsec = nsec_value;
	ventricle_it.it_interval.tv_sec = sec_interval;
	ventricle_it.it_interval.tv_nsec = nsec_interval;

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
			cout << "\npacemaker ran out of battery." << endl;
			break;

		case PACE_HEART_EVENT:
			init_heartbeat();
			break;
		}

	 } else {
		 cout << "\nsomething is wrong!" << endl;
	 }
}


// accepts user input for heart mode
void change_mode() {
	while(1) {
		cout << "\n1 = running    |    2 = normal    |    3 = bradycardia" << endl;
		cout << "Type a number to set the heart mode: ";
		cin >> mode;

		if (mode == 1 || mode == 2 || mode == 3) {
			bpm = mode_bpm[mode-1];
			init_heartbeat();
		} else {
			cout << "invalid entry." << endl;
		}
	}
}


int main() {
	// create a channel to pace heart
	attach = name_attach(NULL, "heart", 0);

	//establish a connection to the pacemaker
	coid = name_open("pacemaker", 0);

	// notify pacemaker heart channel is open
	MsgSend(coid, &coid, sizeof(coid), NULL, sizeof(NULL));

	// initialize heart events and set starting heartbeat = 60bpm
	SIGEV_PULSE_INIT(&ventricle_event, coid, SIGEV_PULSE_PRIO_INHERIT, RIGHT_VENTRICLE_EVENT, 0);
	bpm = 60;
	init_heartbeat();
	cout << "heart is beating..." << endl;

	// create input thread to change heart mode:
	thread user_input(change_mode);

	// receive pace pulse from pacemaker if heartbeat is irregular
	while(1) {
		heart_pace();
	}

    // Close a server connection that was opened by name_open()
	name_close(coid);

	return 0;
}







