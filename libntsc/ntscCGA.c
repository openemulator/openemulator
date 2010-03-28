
/**
 * libntsc
 * CGA NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a CGA NTSC emulation.
 */

#include "ntsc.h"
#include "ntscSignalProcessing.h"

#define NTSC_CGA_COLORNUM	16

double ntscCGAColorIndexToSamples[NTSC_CGA_COLORNUM][4] =
{
	{0, 0, 0, 0},
	{2, 2, 0, 0},
	{0, 0, 2, 2},
	{0, 2, 2, 0},
	{2, 0, 0, 2},
	{1, 2, 1, 0},
	{1, 0, 1, 2},
	{2, 2, 2, 2},

	{1, 1, 1, 1},
	{3, 3, 1, 1},
	{1, 1, 3, 3},
	{1, 3, 3, 1},
	{3, 1, 1, 3},
	{2, 3, 2, 1},
	{2, 1, 2, 3},
	{3, 3, 3, 3},
};

int ntscCGAInit(NTSCCGAData data, NTSCConfiguration *config)
{
	double w[NTSC_CGA_FILTER_N];
	double wy[NTSC_CGA_FILTER_N];
	double wc[NTSC_CGA_FILTER_N];
	double decoderMatrix[NTSC_DECODERMATRIX_SIZE];
	
	if (!calculateChebyshevWindow(w, NTSC_CGA_FILTER_N,
								  NTSC_CGA_CHEBYSHEV_SIDELOBE_DB))
		return 0;
	
	calculateLanczosWindow(wy, NTSC_CGA_FILTER_N, config->lumaBandwidth);
	multiplyWindow(wy, w, NTSC_CGA_FILTER_N);
	
	calculateLanczosWindow(wc, NTSC_CGA_FILTER_N, config->chromaBandwidth);
	multiplyWindow(wc, w, NTSC_CGA_FILTER_N);
	
	copyDecoderMatrix(decoderMatrix, config->decoderMatrix);
	transformDecoderMatrix(decoderMatrix, config->saturation, config->hue);
	
	for (int i = 0; i < NTSC_CGA_CHUNKNUM; i++)
	{
		// i is the input bit sequence to be filtered
		for (int j = 0; j < NTSC_CGA_BLOCKSIZE; j++)
		{
			double yuv[3] = {0.0, 0.0, 0.0};
			double rgb[3];
			
			// Convolution
			for (int k = 0; k < NTSC_CGA_CHUNKSIZE; k++)
			{
				int wi = (NTSC_CGA_BLOCKSIZE - 1) - k;
				if ((wi < 0) || (wi >= NTSC_CGA_FILTER_N))
					continue;

				{
					// Convert CGA data to composite signal level
					int rgbi = (k < 4) ? (i >> 0 & 0xf) : (i >> 4 & 0xf);
					double value = ntscCGAColorIndexToSamples[rgbi][k & 0x3] * 0.25;
					
					yuv[0] += value * wy[wi];
					yuv[1] += value * ntscUPhase[k] * wc[wi];
					yuv[2] += value * ntscVPhase[k] * wc[wi];
				}
			}
			
			applyDecoderMatrix(rgb, yuv, decoderMatrix);
			applyGainAndOffset(rgb, config->brightness, config->contrast);
			applyGainAndOffset(rgb, NTSC_COLORGAIN,
							   (j < NTSC_CGA_CHUNKSIZE) ? NTSC_COLOROFFSET : 0.0);
			
			data[i][j] = NTSC_PACK(rgb[0], rgb[1], rgb[2]);
		}
	}
	
	return 1;
}

#define NTSC_WRITEPIXEL(chunk, index)\
	value = d0[(index + 0) % 24] + \
		d1[(index - 2) % 24] + \
		d2[(index - 4) % 24] + \
		d3[(index - 6) % 24] + \
		d4[(index - 8) % 24] + \
		d5[(index - 10) % 24] + \
		d6[(index - 12) % 24] + \
		d7[(index - 14) % 24] + \
		d8[(index - 16) % 24] + \
		d9[(index - 18) % 24] + \
		d10[(index - 20) % 24] + \
		d11[(index - 22) % 24];\
	NTSC_CLAMP(value);\
	*output++ = NTSC_UNPACK(value);

int ntscCGABlit(NTSCCGAData data,
					int *output, int *input,
					int width, int height)
{
	int chunkNum = width / NTSC_CGA_BLOCKBYTESIZE;
	int chunkPitch = width % NTSC_CGA_BLOCKBYTESIZE;
	
	while (height--)
	{
		int n;
		int *d0 = data[NTSC_CGA_BLACK];
		int *d1 = data[NTSC_CGA_BLACK];
		int *d2 = data[NTSC_CGA_BLACK];
		int *d3 = data[NTSC_CGA_BLACK];
		int *d4 = data[NTSC_CGA_BLACK];
		int *d5 = data[NTSC_CGA_BLACK];
		int *d6 = data[NTSC_CGA_BLACK];
		int *d7 = data[NTSC_CGA_BLACK];
		int *d8 = data[NTSC_CGA_BLACK];
		int *d9 = data[NTSC_CGA_BLACK];
		int *d10 = data[NTSC_CGA_BLACK];
		int *d11;
		int value;
		
		for (n = chunkNum; n; n--)
		{
			d0 = data[*input++];
			NTSC_WRITEPIXEL(data, 0);
			NTSC_WRITEPIXEL(data, 1);
			d1 = data[*input++];
			NTSC_WRITEPIXEL(data, 2);
			NTSC_WRITEPIXEL(data, 3);
			d2 = data[*input++];
			NTSC_WRITEPIXEL(data, 4);
			NTSC_WRITEPIXEL(data, 5);
			d3 = data[*input++];
			NTSC_WRITEPIXEL(data, 6);
			NTSC_WRITEPIXEL(data, 7);
			
			d4 = data[*input++];
			NTSC_WRITEPIXEL(data, 8);
			NTSC_WRITEPIXEL(data, 9);
			d5 = data[*input++];
			NTSC_WRITEPIXEL(data, 10);
			NTSC_WRITEPIXEL(data, 11);
			d6 = data[*input++];
			NTSC_WRITEPIXEL(data, 12);
			NTSC_WRITEPIXEL(data, 13);
			d7 = data[*input++];
			NTSC_WRITEPIXEL(data, 14);
			NTSC_WRITEPIXEL(data, 15);
			
			d8 = data[*input++];
			NTSC_WRITEPIXEL(data, 16);
			NTSC_WRITEPIXEL(data, 17);
			d9 = data[*input++];
			NTSC_WRITEPIXEL(data, 18);
			NTSC_WRITEPIXEL(data, 19);
			d10 = data[*input++];
			NTSC_WRITEPIXEL(data, 20);
			NTSC_WRITEPIXEL(data, 21);
			d11 = data[*input++];
			NTSC_WRITEPIXEL(data, 22);
			NTSC_WRITEPIXEL(data, 23);
		}
		
		if (chunkPitch > 0)
		{
			d0 = data[*input++];
			NTSC_WRITEPIXEL(data, 0);
			NTSC_WRITEPIXEL(data, 1);
			d1 = data[*input++];
			NTSC_WRITEPIXEL(data, 2);
			NTSC_WRITEPIXEL(data, 3);
			d2 = data[*input++];
			NTSC_WRITEPIXEL(data, 4);
			NTSC_WRITEPIXEL(data, 5);
			d3 = data[*input++];
			NTSC_WRITEPIXEL(data, 6);
			NTSC_WRITEPIXEL(data, 7);
			
		}
		if (chunkPitch > 1)
		{
			d4 = data[*input++];
			NTSC_WRITEPIXEL(data, 8);
			NTSC_WRITEPIXEL(data, 9);
			d5 = data[*input++];
			NTSC_WRITEPIXEL(data, 10);
			NTSC_WRITEPIXEL(data, 11);
			d6 = data[*input++];
			NTSC_WRITEPIXEL(data, 12);
			NTSC_WRITEPIXEL(data, 13);
			d7 = data[*input++];
			NTSC_WRITEPIXEL(data, 14);
			NTSC_WRITEPIXEL(data, 15);
		}
	}
	
	return 1;
}
