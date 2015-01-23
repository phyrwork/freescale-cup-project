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
#include <stdlib.h>

static Edge             EdgeBuffer[MAX_NUMBER_OF_TRANSITIONS];
static Line             LineBuffer[MAX_NUMBER_OF_TRANSITIONS + 1];
       Line             TargetLine;
static StopLine         stop;
       PositioningState positioningState;
       int8_t           trackPosition;

#define L 0
#define R 1

void InitTracking(volatile uint16_t* linescan, uint16_t dI_threshold) {

	/* See findPosition() for details about what this lot does */
	int16_t dI[128]; derivative(linescan, dI, 128);

	uint8_t numFeatures = findEdges(dI, dI_threshold);
	        numFeatures = findLines(EdgeBuffer, numFeatures);

	weightLines(&TargetLine, LineBuffer, numFeatures);


	//////////////////////////////////////////////
	// Identify target line based only on width //
	//////////////////////////////////////////////
	
	Line *best = LineBuffer;
	for (uint8_t k = 1; k < numFeatures; ++k)
	{
		Line *candidate = &LineBuffer[k];
		                              
		if (candidate->edges[L].type != EDGE_TYPE_VIRTUAL &&
		    candidate->edges[R].type != EDGE_TYPE_VIRTUAL &&
		    candidate->P_width > best->P_width)
		{
			//Candidate is both fully visible and best match so far
			best = candidate;
		}
	}

	TargetLine = *best; //best match becomes target line
	return;
}

void findPosition(volatile uint16_t* linescan, carState_s* carState, uint16_t dI_threshold)
{
	#define start  edges[0].pos //Macros for easy/readable access to line...
	#define finish edges[1].pos //...start/finish.
	
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
	numFeatures = findLines(EdgeBuffer, numFeatures);
	weightLines(&TargetLine, LineBuffer, numFeatures);


	///////////////////////////////////
	// Look for absolute match first //
	///////////////////////////////////

	Line *best = LineBuffer;
	for (uint8_t k = 1; k < numFeatures; ++k) //select best absolute match
	{
		Line *candidate = &LineBuffer[k];
		                              
		if (candidate->P_absLine > best->P_absLine)
		{
			//Candidate is best absolute match so far
			best = candidate;
		}
	}

	if (best->P_absLine > MIN_CERTAINTY) {
		//Found an absolute match - i.e. a complete line.

		/* Calculate car's track position */
		int8_t center = ((best->start + best->finish)/2) - 64;
		carState->lineCenter = center; //this is offset from car's perspective
		trackPosition = carState->lineCenter; //local copy of track position for telemetry

		TargetLine = *best; //matched line becomes new target line

		/* Update car status; reset timeout counter */
		carState->lineDetectionState = LINE_FOUND;
		TFC_Ticker[3] = 0;

		return;
	}


	////////////////////////////////////
	// Next look for a relative match //
	////////////////////////////////////
	
	//Line *best = LineBuffer;
	best = LineBuffer;
	for (uint8_t k = 1; k < numFeatures; ++k) //select best relative match
	{
		Line *candidate = &LineBuffer[k];
		
		if (candidate->P_relLine > best->P_relLine)
		{
			//Candidate is best relative match so far
			best = candidate;
		}
	}

	if (best->P_relLine > MIN_CERTAINTY) {
		//Found a relative match - i.e. a partial line similar to the target line

		int8_t offset = 0; //New position estimated by change in position of visible edge

		if (best->edges[L].type != EDGE_TYPE_VIRTUAL) {
			//Left edge is not virtual i.e. visible
			positioningState = POSITIONING_STATE_PARTIAL_RIGHT;
			offset = best->edges[L].pos - TargetLine.edges[L].pos;
		}
		else if (best->edges[R].type != EDGE_TYPE_VIRTUAL) {
			//Right edge is not virtual i.e. visible
			positioningState = POSITIONING_STATE_PARTIAL_LEFT;
			offset = best->edges[R].pos - TargetLine.edges[R].pos;
		}

		/* Apply offset to position */
		carState->lineCenter += offset;
		trackPosition = carState->lineCenter; //local copy of road position for telemetry.

		TargetLine = *best; //matched line becomes new target line

		/* Update car status; reset timeout counter */
		carState->lineDetectionState = LINE_FOUND;
		TFC_Ticker[3] = 0;

		return;
	}

	/////////////////////////////////
	// Next look for the stop line //
	/////////////////////////////////

	/*
	if ( findStop(LineBuffer, numFeatures) )
	{
	 	carState->lineDetectionState = STOPLINE_DETECTED;
	 	TFC_ClearLED(1);
	 	TFC_SetLED(2);
	 	TFC_SetLED(3);
	 	return;
	}
	*/
	
	/////////////////////////////////////////////////////////////
	// Before giving up try and re-find the line if it is lost //
	/////////////////////////////////////////////////////////////
	
	//Line *best = LineBuffer;
	best = LineBuffer;
	for (uint8_t k = 1; k < numFeatures; ++k) //select best new match
	{
		Line *candidate = &LineBuffer[k];
									  
		if (candidate->P_newLine > best->P_newLine)
		{
			//Candidate is best absolute match so far
			best = candidate;
		}
	}

	if (best->P_newLine > MIN_CERTAINTY) {
		//Found an absolute match - i.e. a complete line.

		/* Calculate car's track position */
		int8_t center = ((best->start + best->finish)/2) - 64;
		carState->lineCenter = center; //this is offset from car's perspective
		trackPosition = carState->lineCenter; //local copy of track position for telemetry

		TargetLine = *best; //matched line becomes new target line

		/* Update car status; reset timeout counter */
		carState->lineDetectionState = LINE_FOUND;
		TFC_Ticker[3] = 0;

		return;
	}
	
	

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
	
	#undef start
	#undef finish
}

uint8_t findEdges(int16_t* dy, uint16_t threshold)
{
	uint8_t detected = 0; //number of edges found.
	Edge   *edge = &EdgeBuffer[detected]; //pointer to next slot in edge buffer

	for (uint32_t k = 1; k < 128; ++k)
	{		
		//first search for the start of a candidate transition
		for (; k < 128 && !(abs(dy[k]) >= DIFFERENTIAL_THRESHOLD); ++k) {}
		uint8_t start = k;

		//next search for the end of a candidate transition
		int16_t height = 0;
		for (; k < 128 &&
			   abs(dy[k]) > DIFFERENTIAL_THRESHOLD && //differential threshold
			   !( (dy[k] > 0 && dy[k-1] < 0) || (dy[k] < 0 && dy[k-1] > 0) ); //turning points
			   ++k, height += dy[k]) {} //sum differential

		//test candidate
		if ( abs(height) >= HEIGHT_THRESHOLD )
		{
			if (height > 0)
			{
				//rising edge
				edge->pos = start;
				edge->type = EDGE_TYPE_RISING;
			}
			else
			{
				//falling edge
				edge->pos = k - 1;
				edge->type = EDGE_TYPE_FALLING;
			}

			//'save' edge
		    ++detected;
		    ++edge;
		}
	}

	return detected;
}

uint8_t findLines(Edge *edge, uint8_t edges)
{
	uint8_t detected = 0;
	Line   *line = LineBuffer;
	enum {COMPLETE, INCOMPLETE} status;

	//start first line
	if (edges == 0 || edge->type != EDGE_TYPE_RISING)
	{
		//create a virtual edge if no rising edge
		line->edges[L].pos = 0;
		line->edges[L].type = EDGE_TYPE_VIRTUAL;
	}
	else
	{
		//otherwise copy edge and continue
		line->edges[L] = *(edge++);
		--edges; //decrement edges counter
	}
	status = INCOMPLETE; //signal line incomplete

	//analyse remaining edges
	while(edges)
	{
		//search for an alternate edge (skip similar edges)
		while(edges && edge->type == line->edges[L].type)
		{
			++edge;
			--edges;
		}
		if (edges) //if still have edges to complete with
		{
			//complete a line
			line->edges[R] = *(edge++);
			--edges;

			line->width = line->edges[R].pos - line->edges[L].pos;
			++detected;

			status = COMPLETE;
		}

		//start search for a new line
		while(edges && edge->type != EDGE_TYPE_RISING)
		{
			++edge;
			--edges;
		}
		if (edges) //if still have edges to start with
		{
			//start a line
			(++line)->edges[L] = *(edge++);
			--edges;

			status = INCOMPLETE;
		}
	}

	//finish last line
	if (status == INCOMPLETE && line->edges[L].pos < 127)
	{
		line->edges[R].pos = 127;
		line->edges[R].type = EDGE_TYPE_VIRTUAL;

		line->width = line->edges[R].pos - line->edges[L].pos;
		++detected;
	}
}

void weightEdges(Edge* targets, Edge* edges, uint8_t size) {
	
	/* Test each edge against both target edges */
	for (uint8_t t = 0; t < 2; t++)
		for (uint8_t e = 0; e < size; ++e)
		{
			Edge *edge = &edges[e];
			Edge *target = &targets[t];

			//Calculate probability of edge being edge based on change in position
			int16_t dPos = edge->pos - target->pos;
			edge->P_dPos[t] = getProbability(dPos, EDGE_DPOS_SD, EDGE_DPOS_MEAN);
	
			//Calculate probability
			edge->P_edge[t]  = 1;
			edge->P_edge[t] *= edge->P_dPos[t];
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
		
		//New line
		line->P_newLine  = 1;
		line->P_newLine *= line->P_width;
		if (line->edges[L].type == EDGE_TYPE_VIRTUAL ||
			line->edges[R].type == EDGE_TYPE_VIRTUAL)
		{
			//One or both edges isn't visible, so despite width cannot be absolute match
			line->P_newLine = 0;
		}

		//Shared component
		float P_shared  = 1;
		      P_shared *= line->edges[L].P_edge[L];
		      P_shared *= line->edges[R].P_edge[R];

		//Relative line
		line->P_relLine  = P_shared;
		line->P_relLine *= line->P_dWidth;

		//Absolute line
		line->P_absLine  = P_shared;
		line->P_absLine *= line->P_width;
		if (line->edges[L].type == EDGE_TYPE_VIRTUAL ||
			line->edges[R].type == EDGE_TYPE_VIRTUAL)
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
