

#include <iostream>

#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>


using namespace std;


#define RIGHT_ATRIUM_EVENT (_PULSE_CODE_MINAVAIL + 7)
#define RIGHT_VENTRICLE_EVENT (_PULSE_CODE_MINAVAIL + 8)

#define LRI_TIMEOUT (_PULSE_CODE_MINAVAIL + 9)
#define VRP_TIMEOUT (_PULSE_CODE_MINAVAIL + 10)

#define PACE_HEART_EVENT (_PULSE_CODE_MINAVAIL + 11)


typedef union
{
	struct _pulse pulse;
	int id;
} pulse_t;



