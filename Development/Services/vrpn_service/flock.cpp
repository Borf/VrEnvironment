#include "flock.h"

Flock::Flock(char *name, vrpn_Connection *c, int cSensors, char *port, long baud, int fStreamMode, int useERT, bool invertQuaternion)
:vrpn_Tracker_Flock(name, c, cSensors, port, baud, fStreamMode, useERT, invertQuaternion)
{}

Flock::~Flock()
{

}

// mainloop is protected.
void Flock::loop()
{
	mainloop();
}


int Flock::get_report(void)
{
	int ret = vrpn_Tracker_Flock::get_report();

	vrpn_float64 x = pos[0];
	vrpn_float64 y = pos[1];
	vrpn_float64 z = pos[2];

	/*	if(x > 1.3335)
	x = 1.3335;

	if(x < -1.5875)
	x = -1.5875;

	if(y > 1.21666)
	y = 1.21666;

	if(y < -1.4224)
	y = -1.4224;

	if(z > 2.01168)
	z = 2.01168;

	if(z < 0.76708)
	z = 0.76708;*/

	x = (x + 1.5875) * 1.04 - 1.67894;
	y = (y + 1.4224) * 1.15 - 1.52908;
	z = (z - 0.76708) * 1.21 - 0.325628;

	pos[0] = y * -1.0;
	pos[1] = z * -1.0 + 1.5;
	pos[2] = x;

	vrpn_float64 x2 = d_quat[0];
	vrpn_float64 y2 = d_quat[1];
	vrpn_float64 z2 = d_quat[2];
	d_quat[0] = y2;
	d_quat[1] = z2;
	d_quat[2] = x2 * -1;

//	if (buffer[15 - 1] - 1 - d_useERT == 0)
//		printf("0:[%6.2f\t%6.2f\t%6.2f]\t", x, y, z);
//	if (buffer[15 - 1] - 1 - d_useERT == 1)
//		printf("1:[%6.2f\t%6.2f\t%6.2f]\n", x, y, z);
	return ret;
}
