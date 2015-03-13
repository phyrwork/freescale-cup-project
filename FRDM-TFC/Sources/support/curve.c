/*
 * curve.c
 *
 *  Created on: Mar 13, 2015
 *      Author: Connor
 */

float curveApprox(uint32_t const p, float * const x, float * const y, float const in)
{
	if (in <= x[0]) return y[0];
	
	for (uint32_t j = 1; j < p; ++j)
	{
		if(in <= x[j] && in >= x[j-1]) 
		{
			float c = y[j-1];
			float dy = y[j] - y[j-1];
			float dx = x[j] - x[j-1];
			float m = dy/dx;
			
			return c + ( (in - x[j-1]) * m);
		}
	}
	
	return y[p-1];
}
