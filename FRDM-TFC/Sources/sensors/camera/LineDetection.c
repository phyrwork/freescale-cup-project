/*
 * LineDetection.c
 *
 *  Created on: Nov 14, 2013
 *      Author: Matt
 */

#include "sensors/camera/LineDetection.h"
#include "support/ARM_SysTick.h"

static struct detectedTransitions_s lineTransitions = { .numberOfTransitions = 0 };
static struct detectedLines_s detectedLines = { .numberOfLines = 0 };

void findLine(volatile uint16_t* lineScanImage, carState_s* carState, uint16_t derivativeThreshold)
{
	static int16_t imageDifferential[128];
	struct detectedLine_s bestLine = { .certainty = 0 };
	static struct detectedLine_s previousLine;
	lineTransitions.numberOfTransitions = 0;
	detectedLines.numberOfLines = 0;

	getFirstDerivative(lineScanImage, imageDifferential, 128);
	findTransitions(&lineTransitions, imageDifferential, derivativeThreshold);
	transitionsToLines(&detectedLines, &lineTransitions);

	if (detectedLines.numberOfLines == 0)
	{
		if (carState->lineDetectionState != STOPLINE_DETECTED)
		{
			if(TFC_Ticker[3] < LOST_LINE_RESET_DURATION)
			{
				carState->lineDetectionState = LINE_TEMPORARILY_LOST;
			}
			else if(TFC_Ticker[3] > MAX_LOST_LINE_DURATION)
			{
				carState->lineDetectionState = LINE_LOST;
			}
		}
		return;
	}

	//Populate line certainty values
	weightLines(&previousLine, &detectedLines);

	//Select line with highest certainty
	for (uint8_t k = 0; k < detectedLines.numberOfLines; k++)
	{
		if (bestLine.certainty < detectedLines.line[k].certainty)
		{
			bestLine = detectedLines.line[k];
		}
	}

//	TERMINAL_PRINTF("Found line at %i with width %i,\t width certainty %i,\t relative position difference %i and certainty %i,\t relative width difference %i and certainty %i,\t total certainty %i\n", bestLine.center, bestLine.width, (int16_t)(1000.0f*bestLine.widthCertainty), previousLine.center - bestLine.center, (int16_t)(1000.0f*bestLine.relativePositionCertainty), previousLine.width - bestLine.width, (int16_t)(1000.0f*bestLine.relativeWidthCertainty), (int16_t)(1000.0f*bestLine.certainty));

	

	if (bestLine.certainty > MIN_CERTAINTY)
	{
		previousLine = bestLine;
		carState->lineCenter = bestLine.center;
		if (carState->lineDetectionState != STOPLINE_DETECTED)
		{
			carState->lineDetectionState = LINE_FOUND;
		}
		TFC_Ticker[3] = 0;
		return;
	}
	else
	{
		if (TFC_Ticker[3] < LOST_LINE_RESET_DURATION)
		{
			if (carState->lineDetectionState != STOPLINE_DETECTED)
			{
				carState->lineDetectionState = LINE_TEMPORARILY_LOST;
			}
			return;
		}

		bestLine.widthCertainty = 0;

		//Select line with highest certainty based on just width
		for (uint8_t k = 0; k < detectedLines.numberOfLines; k++)
		{
			if (bestLine.widthCertainty < detectedLines.line[k].widthCertainty)
			{
				bestLine = detectedLines.line[k];
			}
		}
		if (bestLine.widthCertainty > MIN_CERTAINTY)
		{
			previousLine = bestLine;
			TFC_Ticker[3] = 0;
			carState->lineCenter = bestLine.center;
			if (carState->lineDetectionState != STOPLINE_DETECTED)
			{
				carState->lineDetectionState = LINE_FOUND;
			}
			return;
		}

		// No line found
		if (carState->lineDetectionState != STOPLINE_DETECTED && TFC_Ticker[3] > MAX_LOST_LINE_DURATION)
		{
			carState->lineDetectionState = LINE_LOST;
		}
		return;
	}
}

int8_t findTransitions(struct detectedTransitions_s* detectedTransitions, int16_t* derivative, uint16_t derivativeThreshold)
{
	//Find and log positions of all transitions and their directions
	for (uint8_t k = 0; k < 128 && detectedTransitions->numberOfTransitions < MAX_NUMBER_OF_TRANSITIONS; k++)
	{
		if (derivative[k] >= derivativeThreshold)
		{
			detectedTransitions->transition[detectedTransitions->numberOfTransitions].position = k;
			detectedTransitions->transition[detectedTransitions->numberOfTransitions].direction = blackToWhite;
//			TERMINAL_PRINTF("end at %i, ", detectedTransitions->transition[detectedTransitions->numberOfTransitions].position);
			detectedTransitions->numberOfTransitions++;
		}
		else if (derivative[k] <= -derivativeThreshold)
		{
			detectedTransitions->transition[detectedTransitions->numberOfTransitions].position = k;
			detectedTransitions->transition[detectedTransitions->numberOfTransitions].direction = whiteToBlack;
//			TERMINAL_PRINTF("start at %i, ", detectedTransitions->transition[detectedTransitions->numberOfTransitions].position);
			detectedTransitions->numberOfTransitions++;
		}
	}
//	TERMINAL_PRINTF("/n");
	return 0;
}

int8_t transitionsToLines(struct detectedLines_s* detectedLines, struct detectedTransitions_s* detectedTransitions)
{
	//Start from first white to black transition and look for pairs, store starts and ends
	for (uint8_t k = 0; k < detectedTransitions->numberOfTransitions && detectedLines->numberOfLines < MAX_NUMBER_OF_LINES; k++)
	{
		if (detectedTransitions->transition[k].direction == whiteToBlack)
		{
			detectedLines->line[detectedLines->numberOfLines].start = detectedTransitions->transition[k].position;
			k++;
			if (detectedTransitions->transition[k].direction == blackToWhite && k < detectedTransitions->numberOfTransitions)
			{
				detectedLines->line[detectedLines->numberOfLines].end = detectedTransitions->transition[k].position;
				detectedLines->line[detectedLines->numberOfLines].width = detectedLines->line[detectedLines->numberOfLines].end
						- detectedLines->line[detectedLines->numberOfLines].start;
				detectedLines->line[detectedLines->numberOfLines].center = detectedLines->line[detectedLines->numberOfLines].start
						+ ((detectedLines->line[detectedLines->numberOfLines].width) / 2) - 64;
				//Successfully found a line
//				TERMINAL_PRINTF("Start: %i, end: %i ", detectedLines->line[detectedLines->numberOfLines].start, detectedLines->line[detectedLines->numberOfLines].end);
				detectedLines->numberOfLines++;
			}
			else
			{
				//Found a second line start
				k--;
			}
		}
	}
//	TERMINAL_PRINTF("\n");
	return 0;
}

int8_t weightLines(struct detectedLine_s* previousLine, struct detectedLines_s* detectedLines)
{
	for (uint8_t k = 0; k < detectedLines->numberOfLines; k++)
	{
		detectedLines->line[k].widthCertainty = getProbability(detectedLines->line[k].width, LINE_WIDTH_SD, LINE_WIDTH_MEAN);

		detectedLines->line[k].relativePositionCertainty = getProbability(previousLine->center - detectedLines->line[k].center, LINE_CENTER_DIFFERENCE_SD,
				LINE_CENTER_DIFFERENCE_MEAN);

		detectedLines->line[k].relativeWidthCertainty = getProbability(previousLine->width - detectedLines->line[k].width, LINE_WIDTH_DIFFERENCE_SD,
				LINE_WIDTH_DIFFERENCE_MEAN);

		detectedLines->line[k].certainty = detectedLines->line[k].widthCertainty * detectedLines->line[k].relativePositionCertainty
				* detectedLines->line[k].relativeWidthCertainty;
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

void getFirstDerivative(volatile uint16_t* input, int16_t* output, uint8_t length)
{

	output[0] = 0;

	for (uint8_t i = 1; i < length; i++)
	{
		output[i] = input[i] - input[i - 1];
	}
}

