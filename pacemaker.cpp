#include <iostream>

#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>


using namespace std;


#define RIGHT_ATRIUM_EVENT (_PULSE_CODE_MINAVAIL + 7)
#define RIGHT_VENTRICLE_EVENT (_PULSE_CODE_MINAVAIL + 8)

typedef union
{
	struct _pulse pulse;
} pulse_t;


int main() {
	name_attach_t *attach;
	pulse_t heartbeat;
	int rcvid;

	// create a channel
	attach = name_attach(NULL, "pacemaker", 0);

	while(1) {
		// receive pulse from heart
		rcvid = MsgReceive(attach->chid, &heartbeat, sizeof(heartbeat), NULL);

		if (0 == rcvid) {
			//received a pulse
			switch (heartbeat.pulse.code) {
			case _PULSE_CODE_DISCONNECT:
				cout << "heart is dead." << endl;
				break;

			case RIGHT_ATRIUM_EVENT:
				cout << "atrium beat" << endl;
				break;

			case RIGHT_VENTRICLE_EVENT:
				cout << "ventricle beat" << endl;
				break;

			default:
				cout << "unknown..." << endl;
				break;
			}

		 } else {
			 cout << "something is wrong!" << endl;
		 }
	}

	return 0;
}