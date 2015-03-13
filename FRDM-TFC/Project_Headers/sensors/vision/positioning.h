/* LineDetection.h
 * ===========================================
 * This library contains the methods required
 * to determine the position of the car 
 * relative to the sides of the track.
 * -------------------------------------------
 * Adapted from the 2014 tracking algorithm by
 * MW. Uses the same principle of probabilites
 * to choose most likely target to follow, but
 * is modified to follow the 2015 competition
 * track with lines at both sides of the
 * course.
 * -------------------------------------------
 * Author: Connor Newton
 * Date:   November 17, 2014
 */

#ifndef LINEDETECTION_H_
#define LINEDETECTION_H_

#include "support/Probability.h"
#include "support/carState_s.h"

#define MAX_NUMBER_OF_TRANSITIONS 8

//findPosition
#define TRACK_DY_T                150
#define TRACK_RY_T                300
#define MAX_LOST_LINE_DURATION    10000
#define LOST_LINE_RESET_DURATION  15000
#define MIN_CERT_ABS 0.3f
#define MIN_CERT_REL 0.3f
#define MIN_CERT_NEW 0.3f

//weightEdges
#define EDGE_DPOS_SD    15
#define EDGE_DPOS_MEAN  0
#define EDGE_D2POS_SD   5
#define EDGE_D2POS_MEAN 0

//weightLines
#define LINE_WIDTH_SD    15
#define LINE_WIDTH_MEAN  95
#define LINE_DWIDTH_SD   10
#define LINE_DWIDTH_MEAN 0

//findStop
#define STOP_DY_T            50
#define STOP_RY_T            100
#define STOP_MIN_CERTAINTY   0.4f
#define STOP_LINE_WIDTH_SD   5
#define STOP_LINE_WIDTH_MEAN 15
#define STOP_LINE_GAP_SD     5
#define STOP_LINE_GAP_MEAN   10
#define STOP_LINE_NOT_FOUND  0
#define STOP_LINE_FOUND      1

typedef uint8_t EdgeType;
#define EDGE_TYPE_RISING  0
#define EDGE_TYPE_FALLING 1
#define EDGE_TYPE_VIRTUAL 255

#define LINE_TYPE_WHITE EDGE_TYPE_RISING
#define LINE_TYPE_BLACK EDGE_TYPE_FALLING

#define TRACK_TYPE_ABS 0
#define TRACK_TYPE_REL 1
#define TRACK_TYPE_NEW 2

typedef struct {
	EdgeType type; //Type of edge (i.e. rising/falling)
	uint8_t  pos;  //Location of edge
	int8_t  dpos; //difference in position from last edge
} Edge_s;

typedef struct {
	Edge_s  edges[2];
	uint8_t width;
	struct {
		float asl; //abs
		float rel;
		float fsh; //new
	} P;
} Line_s;

typedef uint8_t PositioningState;
#define POSITIONING_STATE_FULL 0
#define POSITIONING_STATE_PARTIAL_LEFT 10
#define POSITIONING_STATE_PARTIAL_RIGHT 20
#define POSITIONING_STATE_PARTIAL_NONE 255

void    findPosition(int16_t *dy, carState_s* carState);
uint8_t findEdges(Edge_s *edges, int16_t* dy, uint16_t dy_t, uint16_t ry_t);
uint8_t findLines(Line_s *lines, Edge_s *edges, uint8_t numEdges, uint8_t const type);
void    findTrack(Line_s* line, uint8_t lines);
int8_t  findStop(int16_t *dy);
void    preloadProbabilityTables();
void    diff(volatile uint16_t* input, int16_t* output, uint8_t length);

/* Data sharing for telemetry */
extern PositioningState positioningState; // Line_s tracking status
extern Line_s           BestMatch;
extern Line_s           TargetLine;	// Current model line
extern int8_t           trackPosition;     // Current detected road position

#endif /* LINEDETECTION_H_ */
