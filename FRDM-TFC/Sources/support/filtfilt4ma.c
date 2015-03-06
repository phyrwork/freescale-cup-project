void filtfilt4ma(int16_t *in, int16_t *out)
{
	int16_t buf[128];
	
	//////////////
	// FORWARDS //
	//////////////
	
	buf[0] = in[0] / 4;
	buf[1] = (in[1] + in[0]) / 4;
	buf[2] = (in[2] + in[1] + in[0]) / 4;
	
	for (int j = 3; j < 128; ++j)
	{
		int32_t sam = in[j-0] + in[j-1] + in[j-2] + in[j-3];
		buf[j] = sam / 4;
	}
	
	
	///////////////
	// BACKWARDS //
	///////////////
	
	out[127] = buf[127] / 4;
	out[126] = (buf[126] + buf[127]) / 4;
	out[125] = (buf[125] + buf[126] + buf[127]) / 4;
	
	for (int j = 124; j >= 0; --j)
	{
		int32_t sam = buf[j+0] + buf[j+1] + buf[j+2] + buf[j+3];
		out[j] = sam / 4;
	}
}
