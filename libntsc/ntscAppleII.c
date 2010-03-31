
/**
 * libntsc
 * Apple II NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple II NTSC emulation.
 */

#include "ntsc.h"
#include "ntscSignalProcessing.h"

int ntscAppleIIInit(NTSCAppleIIData data, NTSCConfiguration *config)
{
	double w[NTSC_APPLEII_FILTER_N];
	double wy[NTSC_APPLEII_FILTER_N];
	double wc[NTSC_APPLEII_FILTER_N];
	double decoderMatrix[NTSC_DECODERMATRIX_SIZE];
	
	if (!calculateChebyshevWindow(w, NTSC_APPLEII_FILTER_N,
								  NTSC_APPLEII_CHEBYSHEV_SIDELOBE_DB))
		return 0;
	
	calculateLanczosWindow(wy, NTSC_APPLEII_FILTER_N, config->lumaBandwidth);
	multiplyWindow(wy, w, NTSC_APPLEII_FILTER_N);
	
	calculateLanczosWindow(wc, NTSC_APPLEII_FILTER_N, config->chromaBandwidth);
	multiplyWindow(wc, w, NTSC_APPLEII_FILTER_N);
	
	copyDecoderMatrix(decoderMatrix, config->decoderMatrix);
	transformDecoderMatrix(decoderMatrix, config->saturation, config->hue);
	
	for (int i = 0; i < NTSC_APPLEII_CHUNKNUM; i++)
	{
		// i is the input bit sequence to be filtered
		for (int j = 0; j < NTSC_APPLEII_BLOCKSIZE; j++)
		{
			double yuv[3] = {0.0, 0.0, 0.0};
			double rgb[3];
			
			// Convolution
			for (int k = 0; k < NTSC_APPLEII_CHUNKSIZE; k++)
			{
				int wi = (NTSC_APPLEII_BLOCKSIZE - 1) - k;
				if ((wi < 0) || (wi >= NTSC_APPLEII_FILTER_N))
					continue;
				
				{
					// Convert Apple II bit to composite signal level
					double value = (i >> k) & 1;
				
					yuv[0] += value * wy[wi];
					yuv[1] += value * ntscUPhase[k] * wc[wi];
					yuv[2] += value * ntscVPhase[k] * wc[wi];
				}
			}
			
			applyDecoderMatrix(rgb, yuv, decoderMatrix);
			applyGainAndOffset(rgb, config->brightness, config->contrast);
			applyGainAndOffset(rgb, NTSC_COLORGAIN,
							   (j < NTSC_APPLEII_CHUNKSIZE) ? NTSC_COLOROFFSET : 0.0);
			
			data[i][j] = NTSC_PACK(rgb[0], rgb[1], rgb[2]);
		}
	}
	
	return 1;
}

#define NTSC_WRITEPIXEL(chunk, index)\
	value = d0[(index + 0) % 24] + \
		d1[(index - 8) % 24] + \
		d2[(index - 16) % 24];\
	NTSC_CLAMP(value);\
	*output++ = NTSC_UNPACK(value);

int ntscAppleIIBlit(NTSCAppleIIData data,
					int *output, int *input,
					int width, int height)
{
	int chunkNum = width / NTSC_APPLEII_BLOCKBYTESIZE;
	int chunkPitch = width % NTSC_APPLEII_BLOCKBYTESIZE;
	
	while (height--)
	{
		int n;
		int *d0;
		int *d1 = data[NTSC_APPLEII_BLACK];
		int *d2 = data[NTSC_APPLEII_BLACK];
		int value;
		
		for (n = chunkNum; n; n--)
		{
			d0 = data[*input++];
			NTSC_WRITEPIXEL(data, 0);
			NTSC_WRITEPIXEL(data, 1);
			NTSC_WRITEPIXEL(data, 2);
			NTSC_WRITEPIXEL(data, 3);
			NTSC_WRITEPIXEL(data, 4);
			NTSC_WRITEPIXEL(data, 5);
			NTSC_WRITEPIXEL(data, 6);
			NTSC_WRITEPIXEL(data, 7);
			
			d1 = data[*input++];
			NTSC_WRITEPIXEL(data, 8);
			NTSC_WRITEPIXEL(data, 9);
			NTSC_WRITEPIXEL(data, 10);
			NTSC_WRITEPIXEL(data, 11);
			NTSC_WRITEPIXEL(data, 12);
			NTSC_WRITEPIXEL(data, 13);
			NTSC_WRITEPIXEL(data, 14);
			NTSC_WRITEPIXEL(data, 15);
			
			d2 = data[*input++];
			NTSC_WRITEPIXEL(data, 16);
			NTSC_WRITEPIXEL(data, 17);
			NTSC_WRITEPIXEL(data, 18);
			NTSC_WRITEPIXEL(data, 19);
			NTSC_WRITEPIXEL(data, 20);
			NTSC_WRITEPIXEL(data, 21);
			NTSC_WRITEPIXEL(data, 22);
			NTSC_WRITEPIXEL(data, 23);
		}
		
		if (chunkPitch > 0)
		{
			d0 = data[*input++];
			NTSC_WRITEPIXEL(data, 0);
			NTSC_WRITEPIXEL(data, 1);
			NTSC_WRITEPIXEL(data, 2);
			NTSC_WRITEPIXEL(data, 3);
			NTSC_WRITEPIXEL(data, 4);
			NTSC_WRITEPIXEL(data, 5);
			NTSC_WRITEPIXEL(data, 6);
			NTSC_WRITEPIXEL(data, 7);
		}
		if (chunkPitch > 1)
		{
			d1 = data[*input++];
			NTSC_WRITEPIXEL(data, 0);
			NTSC_WRITEPIXEL(data, 1);
			NTSC_WRITEPIXEL(data, 2);
			NTSC_WRITEPIXEL(data, 3);
			NTSC_WRITEPIXEL(data, 4);
			NTSC_WRITEPIXEL(data, 5);
			NTSC_WRITEPIXEL(data, 6);
			NTSC_WRITEPIXEL(data, 7);
		}
	}
	
	return 1;
}
