#include "sensors/vision/proximity.h"
#include "sensors/vision/positioning.h" //import edge detection
#include "sensors/vision/linescan.h"    //import linescan1
#include "config.h"

#include <math.h>

#define ANGF 54.9f
#define ANGV 57.6f
#define VH   0.225f
#define VN   0.225f
#define P    128

#define SIZEOF_EDGEBUFFER 8
#define PROX_DY_T 100
#define PROX_RY_T 300

float dtable[P];

void InitProximitySensor()
{
	//construct proximity table from given parameters
	for (int i = 0; i < P; ++i)
	{
		dtable[i] = VH * tanf( ANGF - (ANGV/2) + ((2*i)+1)*( ANGV/( (2*P)+1) ) ) - VN;
	}
}

float getProximity()
{
	int16_t dy[128];

	diff(linescan[0].image, dy, 128);
	filtfilt4ma(dy, dy);

	Edge_s edges[SIZEOF_EDGEBUFFER];
	uint8_t features = findEdges(&edges, dy, PROX_DY_T, PROX_RY_T);

	for (int i = 0; i < features; ++i)
	if (edges[i].type == EDGE_TYPE_RISING) return dtable[edges[i].pos];
}
