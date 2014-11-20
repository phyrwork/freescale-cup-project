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

//findLines
#define DIFFERENTIAL_THRESHOLD    300
#define MAX_LOST_LINE_DURATION    10000
#define LOST_LINE_RESET_DURATION  10000
#define MAX_NUMBER_OF_TRANSITIONS 64
#define MAX_NUMBER_OF_STOP_LINES  32
#define MIN_CERTAINTY             0.2f

//weightEdges
#define EDGE_DPOS_SD  20
#define EDGE_DPOS_MEAN 0

//weightLines
#define LINE_WIDTH_SD    10
#define LINE_WIDTH_MEAN  80 //This will need updating to width of track
#define LINE_DWIDTH_SD   15
#define LINE_DWIDTH_MEAN 0

//findStopLine
#define STOP_MIN_CERTAINTY   0.6f
#define STOP_LINE_WIDTH_SD   10
#define STOP_LINE_WIDTH_MEAN 20
#define STOP_GAP_WIDTH_SD    4
#define STOP_GAP_WIDTH_MEAN  4

typedef enum {
	rising,
	falling,
	flat
} EdgeType;

typedef struct {
	EdgeType type; //Type of edge (i.e. rising/falling)
	uint8_t  pos;  //Location of edge
	float    P_dPos[2];
	float    P_edge[2];
} Edge;

typedef struct {
	Edge    edges[2];
  //#define start  edges[0].pos
  //#define finish edges[1].pos
	uint8_t width;
	float   P_width;
	float   P_dWidth;
	float   P_line;
} Line;

typedef enum {
	full,
	partial_L,
	partial_R,
	none
} TrackingState;

typedef struct {
	Line    lines[3];
  //#define lineA lines[0]
  //#define lineB lines[2]
  //#define gap   lines[1]
	float   P_lineA;
	float   P_lineB;
	float   P_gap;
	float   P_stop;
} StopLine;

void    InitTracking(volatile uint16_t* linescan, uint16_t dI_threshold);
void    findPosition(volatile uint16_t* linescan_in, carState_s* carState, uint16_t dI_threshold);
uint8_t findEdges(int16_t* derivative, uint16_t threshold);
uint8_t findLines(Edge* edges, uint8_t numEdges);
void    weightEdges(Edge* targetEdges, Edge* edges, uint8_t numEdges);
int8_t  weightLines(Line* trackedLine, Line* lines, uint8_t numLines);
uint8_t findStop(Line* lines, uint8_t numLines);
void    preloadProbabilityTables();
void    derivative(volatile uint16_t* input, int16_t* output, uint8_t length);



#endif /* LINEDETECTION_H_ */
