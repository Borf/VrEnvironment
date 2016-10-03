#pragma once

#include <vrpn/vrpn_flock.h>


class Flock : public vrpn_Tracker_Flock
{
public:
	Flock(char *name, vrpn_Connection *c, int cSensors=2,
		     char *port = "COM1", long baud = 115200,
		     int fStreamMode = 1, int useERT=1, bool invertQuaternion = false);

	~Flock();

	virtual int get_report(void);
	void loop();
};
