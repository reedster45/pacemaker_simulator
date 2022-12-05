

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
