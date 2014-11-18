/*
 * LineDetection.c
 *
 *  Created on: Nov 14, 2013
 *      Author: Matt
 */

#include "sensors/camera/LineDetection.h"
#include "support/ARM_SysTick.h"
#include <math.h>

static Edge edgeBuffer[MAX_NUMBER_OF_TRANSITIONS];
static Line lineBuffer[MAX_NUMBER_OF_TRANSITIONS + 1];
static Line targetLine;
static TrackingState trackingState;
static detectedLines_s detectedLines = { .numberOfLines = 0 };

#define L 0
#define R 0
#define start  edges[0].pos
#define finish edges[1].pos

void InitTracking(volatile uint16_t* linescan, carState_s* carState, uint16_t dI_threshold) {
	
	/* The following is the same as the prep section of findPosition() */
	static int16_t dI[128]; derivative(linescan, dI, 128);
	static uint8_t numFeatures = findEdges(dI, dI_threshold);
	               numFeatures = findLines(&edgeBuffer, numFeatures);

	weightLines(&targetLine, &lineBuffer, numFeatures);
	uint8_t best = 0;
    #define bestLine lineBuffer[best]
	
	for (uint8_t k = 1; k < numFeatures; k++)
		if (lineBuffer[k].P_width > bestLine.P_width &&
			lineBuffer[k].edges[L] != flat &&
			lineBuffer[k].edges[R] != flat )
				best = k;

	targetLine = bestLine;
	return;
	
    #undef bestLine	
}

void findPosition(volatile uint16_t* linescan, carState_s* carState, uint16_t dI_threshold)
{
	/* If car has 'stopped', nothing to do; return */
	if (carState->lineDetectionState == STOPLINE_DETECTED) return;

	/* Get derivative of linescan image */
	static int16_t dI[128];
	derivative(linescan, dI, 128);

	/* Look for edges and classify */
	static uint8_t numFeatures = 0;
	numFeatures = findEdges(dI, dI_threshold);
	
	/* Generate possible lines for analysis */
	numFeatures = findLines(&edgeBuffer, numFeatures);

	/* Find best match for target line */
	weightLines(&targetLine, &lineBuffer, numFeatures);
	uint8_t best = 0;
	#define bestLine lineBuffer[best]
	
	for (uint8_t k = 1; k < numFeatures; k++)
		if (lineBuffer[k].P_line > bestLine.P_line) best = k;

	/* Decide what to do with the result */
	if (bestLine.P_line > MIN_CERTAINTY) {

		/* Determine if detected line spans the whole track:
		 * if neither of the edges are flat then yes */
		if (bestLine.edges[L].type != flat &&
			bestLine.edges[R].type != flat) {

			trackingState = full;

			/* Calculate car's road position */
			uint8_t center = (bestLine.start + bestLine.finish)/2;
			carState->lineCenter = center;
			//This is the offset of the center of the track from the car's
			//perspective, -not- the offset of the car (i.e. the track position)
		}
		else {
			/* We have detected a track partial - we cannot calculate
			 * the absolute center of the track without edges at both sides:
			 * instead must estimate new position of car using change in edge
			 * positions */
			uint8_t offset;

			/* Choose non-'flat' edge to use to calculate offset */
			if (bestLine.edges[L].type != flat) {
				/* Edge at LHS; LHS partial */
				trackingState = partial_L;
				offset = bestLine.edges[L] - targetLine.edges[L].pos;
			}
			else if (bestLine.edges[R].type != flat) {
				/* Edge at RHS; RHS partial */
				trackingState = partial_R;
				offset = bestLine.edges[R] - targetLine.edges[R].pos;
			}
			/* Apply offset to position */
			carState->lineCenter += offset;
			//This is the offset of the center of the track from the car's
			//perspective, -not- the offset of the car (i.e. the track position)
		}

		/* Store best line as the new target line */
		targetLine = bestLine;

		/* Update car status; reset timeout counter */
		carState->lineDetectionState = LINE_FOUND;
		TFC_Ticker[3] = 0;
	}
	else if (TFC_Ticker[3] > LOST_LINE_RESET_DURATION)
		 carState->lineDetectionState = LINE_LOST;
	else carState->lineDetectionState = LINE_TEMPORARILY_LOST;

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

		/* If large, +ve derivative: rising edge */
		if (derivative[k] >= threshold)
		{
			edgeBuffer[numEdges].pos = k;
			edgeBuffer[numEdges].type = rising;
			numEdges++;
		}
		/* If large, -ve derivative: white->black edge */
		else if (derivative[k] <= -threshold)
		{
			edgeBuffer[numEdges].pos = k;
			numEdges++;
		}
	}
	return numEdges;
}

uint8_t findLines(Edge* edges, uint8_t numEdges)
{
	uint8_t l = 0; //Number of pairs generated
	uint8_t e = 0;

	/* Start constructing first line */
	lineBuffer[l].edges[L].pos = 0;
	lineBuffer[l].edges[L].type = flat;

	/* A line potentially exists between every pair of edges */
	for (e = 0; e < numEdges; e++) {

		/* Make sure we only capture the next edge of a different type */
		EdgeType type = flat;
		if (lineBuffer[l].type == type) continue;
		type = lineBuffer[l].type;

		/* Finish constructing previous line */
		lineBuffer[l].edges[R] = edges[edge];
		lineBuffer[l].width = //Calculate width of line
			lineBuffer[l].finish - lineBuffer[l].start;

		/* Start constructing next line */
		lineBuffer[++lines].edges[L] = edges[edge];
	}

	/* Finish constructing final line */
	lineBuffer[lines].edges[R].pos = 255;
	lineBuffer[lines].edges[R].type = flat;

	return lines;
}

int8_t weightEdges(Edge* targetEdges, Edge* edges, uint8_t numEdges) {
	
	/* Test each edge against both target edges */
	for (uint8_t t = 0; t < 2; t++)
		for (uint8_t e = 0; e < numEdges; e++) {

			/* Calculate probability of edge being edge based on change in
			 * position only
			 */
			int16_t dPos = edges[e].pos - targetEdges[t].pos;
			edges[e].P_dPos[t] = getProbability(dPos, EDGE_DPOS_SD, EDGE_DPOS_MEAN);

			/* Calculate combined probability */
			edges[e].P_edge[t]  = 1;
			edges[e].P_edge[t] *= edges[e].P_dPos[t];
		}
}

int8_t weightLines(Line* targetLine, Line* lines, uint8_t numLines)
{
	for (uint8_t k = 0; k < numLines; k++) {

		/* Calculate probability line is line based on width */
		lines[k].P_width = getProbability( lines[k].width,
								             LINE_WIDTH_SD, LINE_WIDTH_MEAN );

		/* Calculate probabilty line is line based on change in width */
		uint8_t dWidth = lines[k].width - targetLine->width;
		lines[k].P_dWidth = getProbability( dWidth,
								             LINE_DWIDTH_SD, LINE_DWIDTH_MEAN );

		/* Factor likelihood of correct edges */
		weightEdges(targetLine.edges, lines[k].edges, 2);

		/* Calculate combined probability */
		lines[k].P_line  = 1;
		lines[k].P_line *= lines[k].P_dWidth
		lines[k].P_line *= lines[k].edges[L].P_edge[L];
		lines[k].P_line *= lines[k].edges[R].P_edge[R];
	}
	return 0;
}



int8_t weightStopLines(struct stopLines_s* stopLines)
{
	for (uint8_t i = 0; i < stopLines->numberOfStopLines; i++)
	{
		stopLines->stopLine[i].line[0].widthCertainty = getProbability(stopLines->stopLine[i].line[0].width, STOPLINE_SIDE_WIDTH_SD, STOPLINE_SIDE_WIDTH_MEAN);
		stopLines->stopLine[i].line[2].widthCertainty = getProbability(stopLines->stopLine[i].line[2].width, STOPLINE_SIDE_WIDTH_SD, STOPLINE_SIDE_WIDTH_MEAN);
		stopLines->stopLine[i].gap1 = (stopLines->stopLine[i].line[1].center + 64) - stopLines->stopLine[i].line[0].end;
		stopLines->stopLine[i].gap2 = stopLines->stopLine[i].line[2].start - (stopLines->stopLine[i].line[1].center + 64);
		stopLines->stopLine[i].gapWidthCertainty1 = getProbability(stopLines->stopLine[i].gap1, STOPLINE_GAP_WIDTH_SD, STOPLINE_GAP_WIDTH_MEAN);
		stopLines->stopLine[i].gapWidthCertainty2 = getProbability(stopLines->stopLine[i].gap2, STOPLINE_GAP_WIDTH_SD, STOPLINE_GAP_WIDTH_MEAN);

		stopLines->stopLine[i].certainty = stopLines->stopLine[i].line[0].widthCertainty * stopLines->stopLine[i].line[1].widthCertainty
				* stopLines->stopLine[i].line[2].widthCertainty * stopLines->stopLine[i].gapWidthCertainty1 * stopLines->stopLine[i].gapWidthCertainty2;
	}
	return 0;
}

void preloadProbabilityTables()
{
	getProbability(0, STOPLINE_SIDE_WIDTH_SD, STOPLINE_SIDE_WIDTH_MEAN);
	getProbability(0, STOPLINE_GAP_WIDTH_SD, STOPLINE_GAP_WIDTH_MEAN);
}

void findStop(carState_s* carState)
{
	if (detectedLines.numberOfLines < 3)
		return;

	struct stopLines_s stopLines =
	{ .numberOfStopLines = 0 };

	for (int8_t i = 0; i <= detectedLines.numberOfLines - 3; i++)
	{
		for (int8_t j = i + 1; j <= detectedLines.numberOfLines - 2; j++)
		{
			for (int8_t k = j + 1; k <= detectedLines.numberOfLines - 1 && stopLines.numberOfStopLines < MAX_NUMBER_OF_STOP_LINES; k++)
			{
				stopLines.stopLine[stopLines.numberOfStopLines].line[0] = detectedLines.line[i];
				stopLines.stopLine[stopLines.numberOfStopLines].line[1] = detectedLines.line[j];
				stopLines.stopLine[stopLines.numberOfStopLines].line[2] = detectedLines.line[k];
				stopLines.numberOfStopLines++;
			}
		}
	}

	weightStopLines(&stopLines);

//	for (uint8_t i = 0; i < stopLines.numberOfStopLines; i++)
//	{
//		TERMINAL_PRINTF("Found stop line %i with certainty %i, gap certainties %i and %i, side width certainties of %i and %i, center width certainty of %i.\t\t", i, (int16_t)(stopLines.stopLine[i].certainty*1000.0f), (int16_t)(stopLines.stopLine[i].gapWidthCertainty1*1000.0f), (int16_t)(stopLines.stopLine[i].gapWidthCertainty2*1000.0f), (int16_t)(stopLines.stopLine[i].line[0].widthCertainty*1000.0f), (int16_t)(stopLines.stopLine[i].line[2].widthCertainty*1000.0f), (int16_t)(stopLines.stopLine[i].line[1].widthCertainty*1000.0f));
//	}
//	TERMINAL_PRINTF("\r\n");	

	for (uint8_t i = 0; i < stopLines.numberOfStopLines; i++)
	{
		if (stopLines.stopLine[i].certainty > MIN_STOPLINE_CERTAINTY)
		{
			carState->lineDetectionState = STOPLINE_DETECTED;
		}
	}

	return;
}

void derivative(volatile uint16_t* input, int16_t* output, uint8_t length)
{
	output[0] = 0;

	for (uint8_t i = 1; i < length; i++)
		output[i] = input[i] - input[i - 1];
}
