/*
 * Probability.h
 *
 *  Created on: Dec 9, 2013
 *      Author: admin
 */

#ifndef PROBABILITY_H_
#define PROBABILITY_H_

#include <math.h>

#define MAX_NUMBER_OF_PROBABILITY_TABLES 8

#define pi 3.14159f
#define e 2.71828f

struct probabilityTable_s{
	float standardDeviation;
	float probabilities[128];
};

struct probabilityTables_s{
	uint8_t filledEntries;
	struct probabilityTable_s entries[MAX_NUMBER_OF_PROBABILITY_TABLES];
	
};

float getProbability(int8_t position, float standardDeviation, int8_t mean);
void generateProbabilityTable(struct probabilityTable_s* probabilitiesTable, float standardDeviation);



#endif /* PROBABILITY_H_ */
