
#include "support/Probability.h"
#include <math.h>
#include <stdlib.h>
#include "config.h"


struct probabilityTables_s probabilityTables = {.filledEntries = 0};

//Find the probability of a value being correct given a mean and standard deviation
float getProbability(int8_t value, float standardDeviation, int8_t mean)
{
	for(uint8_t i = 0; i < probabilityTables.filledEntries; i++)
	{
		if(probabilityTables.entries[i].standardDeviation == standardDeviation)
		{
			return probabilityTables.entries[i].probabilities[abs(value - mean)];
		}
	}
	//No entry for this SD found, generate table
	
	if(probabilityTables.filledEntries < MAX_NUMBER_OF_PROBABILITY_TABLES)
	{
		TERMINAL_PRINTF("Generating probability table for a standard deviation of %i\n", (int16_t)(standardDeviation*1000.0f));
		generateProbabilityTable(&probabilityTables.entries[probabilityTables.filledEntries], standardDeviation);
		probabilityTables.filledEntries++;
	}
	else
	{
		TERMINAL_PRINTF("Insufficient space for probability arrays, halting");
		while(1);
	}
	//Should successfully get a probability this time
	return getProbability(value, standardDeviation, mean);
	
	//Should never reach here
	TERMINAL_PRINTF("Shit just went horribly wrong, probably overflowed probabilities array\n");
	while(1);
	return -1;
}

void generateProbabilityTable(struct probabilityTable_s* probabilitiesTable, float standardDeviation){
	for(uint8_t i = 0; i < 127; i++)
	{
		probabilitiesTable->probabilities[i] = (0.39894f*(1.0f/standardDeviation)*pow(e,(-pow((i),2.0f))/(2.0f*pow(standardDeviation,2.0f)))) / (0.39894f*(1.0f/standardDeviation)*pow(e,(-pow((0),2.0f))/(2.0f*pow(standardDeviation,2.0f))));
		probabilitiesTable->standardDeviation = standardDeviation;
	}
}
