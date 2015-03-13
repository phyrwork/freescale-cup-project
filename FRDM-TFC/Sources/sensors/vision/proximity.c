#include "sensors/vision/proximity.h"
#include "sensors/vision/positioning.h" //import edge detection
#include "sensors/vision/linescan.h"    //import linescan1
#include "support/filtfilt4ma.h"
#include "support/carState_s.h"
#include "config.h"

#include <math.h>

#define PI 3.14159265359f
#define DEG2RAD(deg) ( (deg/360.0f) * 2 * PI )

#define ANGF DEG2RAD(73.65f)
#define ANGV DEG2RAD(58.69f)
#define VH   0.325f
#define VN   0.225f
#define P    128

#define SIZEOF_EDGEBUFFER 16
#define PROX_DY_T 35
#define PROX_RY_T 150

float dtable[P];

void InitProximitySensor()
{
	//construct proximity table from given parameters
	for (int i = 0; i < P; ++i)
	{
		float ang = ANGF - (ANGV/2) + ((2*i)+1)*( ANGV/( (2*P)+1) );
		if ( ang >= DEG2RAD(90.0f) || ang <= DEG2RAD(-90.0f) ) dtable[i] = INFINITY;
		else dtable[i] = (VH * tanf(ang)) - VN;
	}
}

void updateProximitySensor()
{
	int16_t dy[128];

	diff(linescan[1].image, dy, 128);
	filtfilt4ma(dy, dy);

	Edge_s edges[SIZEOF_EDGEBUFFER];
	uint8_t features = findEdges(edges, dy, PROX_DY_T, PROX_RY_T);

	for (int i = features-1; i >= 0; --i)
	if (edges[i].type == EDGE_TYPE_RISING)
	{
		carState.cornerProximity = dtable[127 - edges[i].pos];
		return;
	}
	
	carState.cornerProximity = INFINITY;
	return;
}
