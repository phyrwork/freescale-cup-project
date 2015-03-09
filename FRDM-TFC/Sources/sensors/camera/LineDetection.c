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
#include "telemetry/Collector.h"

Line_s           BestMatch;
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

	//look for features
	static uint8_t numFeatures = 0;
	numFeatures = findEdges(edges, dy, TRACK_DY_T, TRACK_RY_T);
	numFeatures = findLines(lines, edges, numFeatures, LINE_TYPE_WHITE);
	findTrack(lines, numFeatures);
	
	
	//////////////////////////////////
	// Find best match of each type //
	//////////////////////////////////
	
	Line_s *asl = lines;
	Line_s *rel = lines;
	Line_s *fsh = lines;
	for (uint8_t i = 0; i < numFeatures; ++i)
	{
		asl = lines[i].P.asl > asl->P.asl ? &lines[i] : asl;
		rel = lines[i].P.rel > rel->P.rel ? &lines[i] : rel;
		fsh = lines[i].P.fsh > fsh->P.fsh ? &lines[i] : fsh;
	}
	
	////////////////////////////////////////////////////////
	// Choose suitable line - prioritise best match types //
	////////////////////////////////////////////////////////
	
	Line_s *best = 0;
	        best = fsh->P.fsh >= MIN_CERT_NEW ? fsh : best;
	        best = rel->P.rel >= MIN_CERT_REL ? rel : best;
	        best = asl->P.asl >= MIN_CERT_ABS ? asl : best;
	
	if (best)
	{
		//////////////////////////////////////////////////////
		// Found a good match for at least one type of line //
		//////////////////////////////////////////////////////
		
		if (best->edges[L].type != EDGE_TYPE_VIRTUAL &&
			best->edges[R].type != EDGE_TYPE_VIRTUAL)
		{
			//found an absolute match - i.e. a complete line.

			//calculate car's track position
			int8_t center = ((best->start + best->finish)/2) - 64;
			carState->lineCenter = center; //this is offset from car's perspective
			trackPosition = carState->lineCenter; //local copy of track position for telemetry
		}
		else
		{
			//Found a relative match - i.e. a partial line similar to the target line

			int8_t offset = 0; //New position estimated by change in position of visible edge

			if (best->edges[L].type != EDGE_TYPE_VIRTUAL)
			{
				//Left edge is not virtual i.e. visible
				positioningState = POSITIONING_STATE_PARTIAL_RIGHT;
				offset = best->edges[L].pos - TargetLine.edges[L].pos;
			}
			else if (best->edges[R].type != EDGE_TYPE_VIRTUAL)
			{
				//Right edge is not virtual i.e. visible
				positioningState = POSITIONING_STATE_PARTIAL_LEFT;
				offset = best->edges[R].pos - TargetLine.edges[R].pos;
			}

			//Apply offset to position
			carState->lineCenter += offset;
			trackPosition = carState->lineCenter; //local copy of road position for telemetry.
		}
		
		BestMatch  = *best;
		TargetLine = *best; //matched line becomes new target line
		CollectorRequest(BEST_MATCH_COLLECTOR_INDEX);
		CollectorRequest(TARGET_LINE_COLLECTOR_INDEX);
				
		//Update car status; reset timeout counter
		carState->lineDetectionState = LINE_FOUND;
		TFC_Ticker[3] = 0;
	}
	else
	{
		//////////////////////////////////////////	
		// Didn't find a good match - line lost //
		//////////////////////////////////////////
		
		best = rel->P.rel > fsh->P.fsh ? rel : fsh;
		best = asl->P.asl > best->P.rel ? asl : best;
		BestMatch  = *best;
		CollectorRequest(BEST_MATCH_COLLECTOR_INDEX);

		if (TFC_Ticker[3] > LOST_LINE_RESET_DURATION)
		{
			 carState->lineDetectionState = LINE_LOST;
			 TFC_ClearLED(1);
			 TFC_ClearLED(2);
			 TFC_SetLED(3);
		}
		else
		{
			carState->lineDetectionState = LINE_TEMPORARILY_LOST;
			TFC_ClearLED(1);
			TFC_SetLED(2);
			TFC_ClearLED(3);
		}	
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

void findTrack(Line_s* line, uint8_t lines)
{
	Line_s * const target = &TargetLine;

	for(; lines; --lines)
	{
		//calculate dpos
		line->edges[L].dpos = line->edges[L].pos - target->edges[L].pos;
		line->edges[R].dpos = line->edges[R].pos - target->edges[R].pos;

		//count virtual edges
		uint8_t ve = 0;
		if (line->edges[L].type == EDGE_TYPE_VIRTUAL) ++ve;
		if (line->edges[R].type == EDGE_TYPE_VIRTUAL) ++ve;


		//individual probs
		float P_w;
		float P_dw;
		float P_dp_l;
		float P_d2p_l;
		float P_dp_r;
		float P_d2p_r;

		if (ve < 1)
		{
			P_w = getProbability(line->width, LINE_WIDTH_SD, LINE_WIDTH_MEAN);
		}

		if (ve < 2)
		{
			P_dw = getProbability(target->width - line->width, LINE_DWIDTH_SD, LINE_DWIDTH_MEAN);
		}

		if (ve == 1)
		{
			P_dp_l  = getProbability(line->edges[L].dpos, EDGE_DPOS_SD, EDGE_DPOS_MEAN);
			P_d2p_l = getProbability(line->edges[L].dpos - target->edges[L].dpos, EDGE_D2POS_SD, EDGE_D2POS_MEAN);
			P_dp_r  = getProbability(line->edges[R].dpos, EDGE_DPOS_SD, EDGE_DPOS_MEAN);
			P_d2p_r = getProbability(line->edges[R].dpos - target->edges[R].dpos, EDGE_D2POS_SD, EDGE_D2POS_MEAN);
		}


		//combined probs
		line->P.fsh = ve == 0 ? P_w : 0;
		line->P.asl = ve == 0 ? P_w * P_dw : 0;
		line->P.rel = ve == 1 ? P_dw * P_dp_l * P_dp_r /* * P_d2p_l * P_d2p_r */ : 0;
	}
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
