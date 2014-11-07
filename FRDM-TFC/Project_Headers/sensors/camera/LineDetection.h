/*
 * LineDetection.h
 *
 *  Created on: Nov 14, 2013
 *      Author: Matt
 */

#ifndef LINEDETECTION_H_
#define LINEDETECTION_H_

#include "support/Probability.h"
#include "support/carState_s.h"

//findLine
#define DIFFERENTIAL_THRESHOLD 300
#define LINE_CENTER_DIFFERENCE_SD 10
#define LINE_CENTER_DIFFERENCE_MEAN 0
#define LINE_WIDTH_DIFFERENCE_SD 5
#define LINE_WIDTH_DIFFERENCE_MEAN 0
#define LINE_WIDTH_SD 5
#define LINE_WIDTH_MEAN 6
#define MIN_CERTAINTY 0.2f
#define MAX_LOST_LINE_DURATION 10000
#define LOST_LINE_RESET_DURATION 10000
#define MAX_NUMBER_OF_TRANSITIONS 64
#define MAX_NUMBER_OF_LINES 32
#define MAX_NUMBER_OF_STOP_LINES 32

//findStopLine
#define MIN_STOPLINE_CERTAINTY 0.3f
#define STOPLINE_SIDE_WIDTH_SD 10
#define STOPLINE_SIDE_WIDTH_MEAN 20
#define STOPLINE_GAP_WIDTH_SD 4
#define STOPLINE_GAP_WIDTH_MEAN 4

typedef enum {blackToWhite, whiteToBlack} transitionDirection_t;

struct detectedTransition_s{
	uint8_t position;
	transitionDirection_t direction;
};

struct detectedTransitions_s{
	struct detectedTransition_s transition[MAX_NUMBER_OF_TRANSITIONS];
	uint8_t numberOfTransitions;
};

struct detectedLine_s{
	uint8_t start;
	uint8_t end;
	uint8_t width;
	int8_t center;
	float certainty;
	float widthCertainty;
	float relativePositionCertainty;
	float relativeWidthCertainty;
};

struct detectedLines_s{
	struct detectedLine_s line[MAX_NUMBER_OF_LINES];
	uint8_t numberOfLines;
};

struct stopLine_s{
	struct detectedLine_s line[3]; 	//Shouldn't actually need multiple copies of the lines, pointers to their existing 
									//locations would suffice. Must however be careful to ensure that the new width
									//probabilities don't start overwriting each other.
	int8_t gap1;
	int8_t gap2;
	float gapWidthCertainty1;
	float gapWidthCertainty2;
	float certainty;
};

struct stopLines_s{
	struct stopLine_s stopLine[MAX_NUMBER_OF_STOP_LINES];
	uint8_t numberOfStopLines;
};

void findLine(volatile uint16_t* lineScanImage, carState_s* carState, uint16_t derivativeThreshold);
int8_t findTransitions(struct detectedTransitions_s* detectedTransitions, int16_t* derivative, uint16_t derivativeThreshold);
int8_t transitionsToLines(struct detectedLines_s* detectedLines, struct detectedTransitions_s* detectedTransitions);
int8_t weightLines(struct detectedLine_s* previousLine, struct detectedLines_s* detectedLines);
void preloadProbabilityTables();
void findStop(carState_s* carState);
void getFirstDerivative(volatile uint16_t* input, int16_t* output, uint8_t length);



#endif /* LINEDETECTION_H_ */
