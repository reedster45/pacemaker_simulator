#ifndef QNX_H_
#define QNX_H_

#include <iostream>
#include <string>

#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
using namespace std;

typedef union
{
	struct _pulse pulse;
	int id;
} pulse_t;




#endif /* QNX_H_ */
