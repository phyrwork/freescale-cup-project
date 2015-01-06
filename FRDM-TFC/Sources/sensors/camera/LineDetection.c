/* LineDetection.c
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

#include "sensors/camera/LineDetection.h"
#include "support/ARM_SysTick.h"
#include "devices/TFC_SHIELD.h"
#include <math.h>

static Edge             edgeBuffer[MAX_NUMBER_OF_TRANSITIONS];
static Line             lineBuffer[MAX_NUMBER_OF_TRANSITIONS + 1];
       Line             targetLine;
static StopLine         stop;
       PositioningState positioningState;
       int8_t           trackPosition;

#define L 0
#define R 1
#define start  edges[0].pos //Macros for easy/readable access to line...
#define finish edges[1].pos //...start/finish.

void InitTracking(volatile uint16_t* linescan, uint16_t dI_threshold) {
	
	/* See findPosition() for details about what this lot does */
	static int16_t dI[128]; derivative(linescan, dI, 128);
	static uint8_t numFeatures;
	               numFeatures = findEdges(dI, dI_threshold);
	               numFeatures = findLines(edgeBuffer, numFeatures);

	weightLines(&targetLine, lineBuffer, numFeatures);
	uint8_t best = 0;
    #define bestLine lineBuffer[best]
	
	/* Identify correct 'line' */
	for (uint8_t k = 1; k < numFeatures; k++)
		if (lineBuffer[k].P_width > bestLine.P_width &&
			lineBuffer[k].edges[L].type != EDGE_TYPE_VIRTUAL &&
			lineBuffer[k].edges[R].type != EDGE_TYPE_VIRTUAL )
				best = k;

	targetLine = bestLine;
	return;

    #undef bestLine	
}

void findPosition(volatile uint16_t* linescan, carState_s* carState, uint16_t dI_threshold)
{
	/* If car has 'stopped', nothing to do; return */
	// To-do: Move this check to main()
	if (carState->lineDetectionState == STOPLINE_DETECTED) return;


	//////////////////////////////////////////
	// Analyse most recently captured image //
	//////////////////////////////////////////

	/* Get derivative of image */
	static int16_t dI[128];
	derivative(linescan, dI, 128);

	/* Look for edges and classify */
	static uint8_t numFeatures = 0;
	numFeatures = findEdges(dI, dI_threshold);
	
	/* Generate lines and analyse */
	numFeatures = findLines(edgeBuffer, numFeatures);
	weightLines(&targetLine, lineBuffer, numFeatures);


	///////////////////////////////////
	// Look for absolute match first //
	///////////////////////////////////

	uint8_t best = 0;
	#define bestLine lineBuffer[best]
	for (uint8_t k = 1; k < numFeatures; k++) //select best absolute match
		if (lineBuffer[k].P_absLine > bestLine.P_absLine) best = k;

	if (bestLine.P_absLine > MIN_CERTAINTY) {
		//Found an absolute match - i.e. a complete line.

		/* Calculate car's track position */
		int8_t center = ((bestLine.start + bestLine.finish)/2) - 64;
		carState->lineCenter = center; //this is offset from car's perspective
		trackPosition = carState->lineCenter; //local copy of track position for telemetry

		targetLine = bestLine; //matched line becomes new target line

		/* Update car status; reset timeout counter */
		carState->lineDetectionState = LINE_FOUND;
		TFC_Ticker[3] = 0;

		return;
	}
	

	////////////////////////////////////
	// Next look for a relative match //
	////////////////////////////////////

	uint8_t best = 0;
	for (uint8_t k = 1; k < numFeatures; k++) //select best absolute match
		if (lineBuffer[k].P_relLine > bestLine.P_relLine) best = k;

	if if (bestLine.P_relLine > MIN_CERTAINTY) {
		//Found a relative match - i.e. a partial line similar to the target line

		int8_t offset; //New position estimated by change in position of visible edge

		if (bestLine.edges[L].type != EDGE_TYPE_VIRTUAL) {
			//Left edge is not virtual i.e. visible
			positioningState = POSITIONING_STATE_PARTIAL_RIGHT;
			offset = bestLine.edges[L].pos - targetLine.edges[L].pos;
		}
		else if (bestLine.edges[R].type != EDGE_TYPE_VIRTUAL) {
			//Right edge is not virtual i.e. visible
			positioningState = POSITIONING_STATE_PARTIAL_LEFT;
			offset = bestLine.edges[R].pos - targetLine.edges[R].pos;
		}

		/* Apply offset to position */
		carState->lineCenter += offset;
		trackPosition = carState->lineCenter; //local copy of road position for telemetry.

		targetLine = bestLine; //matched line becomes new target line

		/* Update car status; reset timeout counter */
		carState->lineDetectionState = LINE_FOUND;
		TFC_Ticker[3] = 0;

		return;
	}

	/////////////////////////////////
	// Next look for the stop line //
	/////////////////////////////////

	/*
	if ( findStop(lineBuffer, numFeatures) )
	{
	 	carState->lineDetectionState = STOPLINE_DETECTED;
	 	TFC_ClearLED(1);
	 	TFC_SetLED(2);
	 	TFC_SetLED(3);
	 	return;
	}
	*/

	/////////////////////////////////////////////////////	
	// And if not this then probably just noise or car //
	// is traversing a cross-over section              //
	/////////////////////////////////////////////////////

	if (TFC_Ticker[3] > LOST_LINE_RESET_DURATION) {
		 carState->lineDetectionState = LINE_LOST;
		 TFC_ClearLED(1);
		 TFC_ClearLED(2);
		 TFC_SetLED(3);
	}
 	else {
 		carState->lineDetectionState = LINE_TEMPORARILY_LOST;
 		TFC_ClearLED(1);
 		TFC_SetLED(2);
 		TFC_ClearLED(3);
 	}

	return;

	#undef bestLine
}

uint8_t findEdges(int16_t* derivative, uint16_t threshold)
{
	uint8_t numEdges = 0; //Number of edges found.

	/* Find and log positions of all transitions and their directions */
	for (uint8_t k = 0;
		 k < 128 && numEdges < MAX_NUMBER_OF_TRANSITIONS;
		 k++) {

		/* If large, +ve derivative: EDGE_TYPE_RISING edge */
		if (derivative[k] >= threshold)
		{
			edgeBuffer[numEdges].pos = k;
			edgeBuffer[numEdges].type = EDGE_TYPE_RISING;
			numEdges++;
		}
		/* If large, -ve derivative: white->black edge */
		else if (derivative[k] <= -threshold)
		{
			edgeBuffer[numEdges].pos = k;
			edgeBuffer[numEdges].type = EDGE_TYPE_FALLING;
			numEdges++;
		}
	}
	return numEdges;
}

uint8_t findLines(Edge* edges, uint8_t numEdges)
{
	uint8_t l = 0; //Number of pairs generated

	/* Start constructing first line */
	lineBuffer[l].edges[L].pos = 0;
	lineBuffer[l].edges[L].type = EDGE_TYPE_VIRTUAL;
	
	/* A line potentially exists between every pair of edges */
	for (uint8_t e = 0; e < numEdges; e++) {

		/* Make sure we only capture the next edge of a different type */
		
		if (edges[e].type == lineBuffer[l].edges[L].type) continue;

		/* Finish constructing previous line */
		lineBuffer[l].edges[R] = edges[e];
		lineBuffer[l].width = //Calculate width of line
			lineBuffer[l].finish - lineBuffer[l].start;

		/* Start constructing next line */
		lineBuffer[++l].edges[L] = edges[e];
	}

	/* Finish constructing final line */
	lineBuffer[l].edges[R].pos = 127;
	lineBuffer[l].edges[R].type = EDGE_TYPE_VIRTUAL;
	lineBuffer[l].width = //Calculate width of line
				lineBuffer[l].finish - lineBuffer[l].start;

	return ++l;
}

void weightEdges(Edge* targetEdges, Edge* edges, uint8_t numEdges) {
	
	/* Test each edge against both target edges */
	for (uint8_t t = 0; t < 2; t++)
		for (uint8_t e = 0; e < numEdges; e++) {
			/* Calculate probability of edge being edge based on change in
			 * position only
			 */
			int16_t dPos = edges[e].pos - targetEdges[t].pos;
			edges[e].P_dPos[t] = getProbability(dPos, EDGE_DPOS_SD, EDGE_DPOS_MEAN);
	
			/* Calculate probability */
			edges[e].P_edge[t]  = 1;
			edges[e].P_edge[t] *= edges[e].P_dPos[t];
		}
	
	return;
}

int8_t weightLines(Line* target, Line* lines, uint8_t size)
{
	for (uint8_t k = 0; k < size; ++k) {

		Line *line = &lines[k]; //pointer to candidate line

		////////////////////////////////////////
		// Calculate individual probabilities //
		////////////////////////////////////////

		line->P_width = getProbability(line->width, LINE_WIDTH_SD, LINE_WIDTH_MEAN); //P based on abs width

		uint8_t dWidth = line->width - target->width;
		line->P_dWidth = getProbability(dWidth, LINE_DWIDTH_SD, LINE_DWIDTH_MEAN); //P based on change in width

		weightEdges(target->edges, line->edges, 2); //P of constituent edges


		//////////////////////////////////////
		// Calculate combined probabilities //
		//////////////////////////////////////

		//Shared component
		float P_shared  = 1;
		      P_shared *= line->edges[L].P_edge[L];
		      P_shared *= line->edges[R].P_edge[R];

		//Relative line
		line->P_relLine  = P_shared;
		line->P_relLine *= line->P_dWidth

		//Absolute line
		line->P_absLine  = P_shared;
		line->P_absline *= line->P_width;
		if (line->edges[L].type == EDGE_TYPE_VIRTUAL ||
			line0>edges[R].type == EDGE_TYPE_VIRTUAL)
		{
			//One or both edges isn't visible, so despite width cannot be absolute match
			line->P_absLine = 0;
		}

	}
	return 0;
}

 #define lineA lines[0]
 #define lineB lines[2]
 #define gap   lines[1]

uint8_t findStop(Line* lines, uint8_t numLines)
{
	/* Iterate though groups of 3 consecutive lines
	 * --------------------------------------------
	 * [0] Line A (Black)
	 * [1] Gap    (White)
	 * [2] Line B (Black)
	 */
	for (uint8_t i = 0; i < numLines - 3; i++) {

		/* Gather data */
		for (uint8_t l = 0; l < 3; l++) stop.lines[l] = lines[i + l];

		/* Calculate probabilities */
		stop.P_lineA = getProbability(stop.lineA.width, STOP_LINE_WIDTH_SD, STOP_LINE_WIDTH_MEAN);
		stop.P_lineB = getProbability(stop.lineB.width, STOP_LINE_WIDTH_SD, STOP_LINE_WIDTH_MEAN);
		stop.P_gap   = getProbability(stop.gap.width, STOP_GAP_WIDTH_SD, STOP_GAP_WIDTH_MEAN);

		/* Combine probabilities */
		stop.P_stop  = 1;
		stop.P_stop *= stop.P_lineA;
		stop.P_stop *= stop.P_lineB;
		stop.P_stop *= stop.P_gap;

		/* Compare to trust threshold */
		if (stop.P_stop > STOP_MIN_CERTAINTY) return 1; //Return true; STOP THE CAR!
		else return 0;
	}
	return 1;
}

void preloadProbabilityTables()
{
	getProbability(0, STOP_LINE_WIDTH_SD, STOP_LINE_WIDTH_MEAN);
	getProbability(0, STOP_GAP_WIDTH_SD, STOP_GAP_WIDTH_MEAN);
}	

void derivative(volatile uint16_t* input, int16_t* output, uint8_t length)
{
	output[0] = 0;

	for (uint8_t i = 1; i < length; i++)
		output[i] = input[i] - input[i - 1];
}
