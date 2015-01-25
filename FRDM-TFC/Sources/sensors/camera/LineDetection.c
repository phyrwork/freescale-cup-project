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

       Line_s           TargetLine;
       PositioningState positioningState;
       int8_t           trackPosition;

#define L 0
#define R 1

void findPosition(int16_t *dy, carState_s* carState)
{
	#define start  edges[0].pos //Macros for easy/readable access to line...
	#define finish edges[1].pos //...start/finish.

	Edge_s edges[MAX_NUMBER_OF_TRANSITIONS];
	Line_s lines[MAX_NUMBER_OF_TRANSITIONS + 1];

	//////////////////////////////////////////
	// Analyse most recently captured image //
	//////////////////////////////////////////

	/* Look for edges and classify */
	static uint8_t numFeatures = 0;
	numFeatures = findEdges(edges, dy, TRACK_DY_T, TRACK_RY_T);
	
	/* Generate lines and analyse */
	numFeatures = findLines(lines, edges, numFeatures, LINE_TYPE_WHITE);
	weightLines(&TargetLine, lines, numFeatures);


	///////////////////////////////////
	// Look for absolute match first //
	///////////////////////////////////

	Line_s *best = lines;
	for (uint8_t k = 1; k < numFeatures; ++k) //select best absolute match
	{
		Line_s *candidate = &lines[k];
		                              
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
	
	//Line_s *best = lines;
	best = lines;
	for (uint8_t k = 1; k < numFeatures; ++k) //select best relative match
	{
		Line_s *candidate = &lines[k];
		
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
	if ( findStop(lines, numFeatures) )
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
	
	//Line_s *best = lines;
	best = lines;
	for (uint8_t k = 1; k < numFeatures; ++k) //select best new match
	{
		Line_s *candidate = &lines[k];
									  
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

uint8_t findEdges(Edge_s *edges, int16_t* dy, uint16_t dy_t, uint16_t ry_t)
{
	uint8_t detected = 0; //number of edges found.
	Edge_s   *edge = &edges[detected]; //pointer to next slot in edge buffer

	for (uint32_t k = 1; k < 128; ++k)
	{		
		//first search for the start of a candidate transition
		for (; k < 128 && !(abs(dy[k]) >= dy_t); ++k) {}
		uint8_t start = k;

		//next search for the end of a candidate transition
		int16_t height = 0;
		for (; k < 128 &&
			   abs(dy[k]) > dy_t && //differential threshold
			   !( (dy[k] > 0 && dy[k-1] < 0) || (dy[k] < 0 && dy[k-1] > 0) ); //turning points
			   ++k, height += dy[k]) {} //sum differential

		//test candidate
		if ( abs(height) >= ry_t )
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

uint8_t findLines(Line_s *lines, Edge_s *edge, uint8_t edges, uint8_t const type)
{
	uint8_t detected = 0;
	Line_s *line = lines;
	enum {COMPLETE, INCOMPLETE} status;

	//start first line
	if (edges == 0 || edge->type != type)
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
		while(edges && edge->type != type)
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
	
	return detected;
}

void weightEdges(Edge_s* targets, Edge_s* edges, uint8_t size) {
	
	/* Test each edge against both target edges */
	for (uint8_t t = 0; t < 2; t++)
		for (uint8_t e = 0; e < size; ++e)
		{
			Edge_s *edge = &edges[e];
			Edge_s *target = &targets[t];

			//Calculate probability of edge being edge based on change in position
			int16_t dPos = edge->pos - target->pos;
			edge->P_dPos[t] = getProbability(dPos, EDGE_DPOS_SD, EDGE_DPOS_MEAN);
	
			//Calculate probability
			edge->P_edge[t]  = 1;
			edge->P_edge[t] *= edge->P_dPos[t];
		}
	
	return;
}

int8_t weightLines(Line_s* target, Line_s* lines, uint8_t size)
{
	for (uint8_t k = 0; k < size; ++k) {

		Line_s *line = &lines[k]; //pointer to candidate line

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

int8_t findStop(int16_t *dy)
{
	//identify edges
	Edge_s  edges[MAX_NUMBER_OF_TRANSITIONS];
	uint8_t features = findEdges(edges, dy, STOP_DY_T, STOP_RY_T);

	//identify lines
	Line_s  lines[MAX_NUMBER_OF_TRANSITIONS + 1];
	        features = findLines(lines, edges, features, LINE_TYPE_BLACK);

	//analyse lines
	if (features < 2) return STOP_LINE_NOT_FOUND;
	for (uint8_t i = 1; i < features; ++i)
	{
		//test separation
		uint8_t gap = lines[i].edges[L].pos - lines[i-1].edges[R].pos;
		float p_gap = getProbability(gap, STOP_LINE_GAP_SD, STOP_LINE_GAP_MEAN);
		if (p_gap < STOP_MIN_CERTAINTY) continue;

		//test widths
		float p_width = getProbability(lines[i-1].width, STOP_LINE_WIDTH_SD, STOP_LINE_GAP_MEAN);
		if (p_width < STOP_MIN_CERTAINTY) continue;

		p_width = getProbability(lines[i].width, STOP_LINE_WIDTH_SD, STOP_LINE_GAP_MEAN);
		if (p_width < STOP_MIN_CERTAINTY) continue;

		//all tests satisfied
		return STOP_LINE_FOUND;
	}

	return STOP_LINE_NOT_FOUND;
}

void preloadProbabilityTables()
{
	getProbability(0, STOP_LINE_WIDTH_SD, STOP_LINE_WIDTH_MEAN);
	getProbability(0, STOP_LINE_GAP_SD, STOP_LINE_GAP_MEAN);
}	

void diff(volatile uint16_t* input, int16_t* output, uint8_t length)
{
	output[0] = 0;

	for (uint8_t i = 1; i < length; i++)
		output[i] = input[i] - input[i - 1];
}
