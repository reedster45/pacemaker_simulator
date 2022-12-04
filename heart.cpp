#include <iostream>

#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>


using namespace std;


#define RIGHT_ATRIUM_EVENT (_PULSE_CODE_MINAVAIL + 7)
#define RIGHT_VENTRICLE_EVENT (_PULSE_CODE_MINAVAIL + 8)

int main() {
	int coid;
	struct sigevent atrium_event, ventricle_event;
	timer_t atrium_timerid, ventricle_timerid;
	struct itimerspec atrium_it, ventricle_it;

	//establish a connection to the server's channel
	coid = name_open("pacemaker", 0);



	// setup right atrium timer
	SIGEV_PULSE_INIT(&atrium_event, coid, SIGEV_PULSE_PRIO_INHERIT, RIGHT_ATRIUM_EVENT, 0);
	timer_create(CLOCK_REALTIME, &atrium_event, &atrium_timerid);

	atrium_it.it_value.tv_sec = 5;
	atrium_it.it_value.tv_nsec = 0;
	atrium_it.it_interval.tv_sec = 1;
	atrium_it.it_interval.tv_nsec = 500000000;

	timer_settime(atrium_timerid, 0, &atrium_it, NULL);





	// setup right ventricle timer
	SIGEV_PULSE_INIT(&ventricle_event, coid, SIGEV_PULSE_PRIO_INHERIT, RIGHT_VENTRICLE_EVENT, 0);
	timer_create(CLOCK_REALTIME, &ventricle_event, &ventricle_timerid);

	ventricle_it.it_value.tv_sec = 3;
	ventricle_it.it_value.tv_nsec = 0;
	ventricle_it.it_interval.tv_sec = 1;
	ventricle_it.it_interval.tv_nsec = 500000000;

	timer_settime(ventricle_timerid, 0, &ventricle_it, NULL);


	while(1) {
		cout << "heart is beating..." << endl;
		sleep(10);
	}

    // Close a server connection that was opened by name_open()
	name_close(coid);

	return 0;
}



