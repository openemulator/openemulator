
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

void calculateAppleIISignalToPixel(NTSCCGASignalToPixel signalToPixel,
								   NTSCConfiguration *config,
								   double *wy, double *wc, double *decoderMatrix)
{
	for (int is = 0; is < NTSC_APPLEII_SIGNALSIZE; is++)
	{
		for (int ib = 0; ib < NTSC_APPLEII_BLOCKSIZE; ib++)
		{
			double yuv[3] = {0.0, 0.0, 0.0};
			double rgb[3];
			
			// Convolution
			for (int ic = 0; ic < NTSC_APPLEII_CHUNKSIZE; ic++)
			{
				int iw = ib - ic;
				if ((iw >= 0) && (iw < NTSC_APPLEII_FILTER_N))
				{
					// Calculate composite signal level
					double signal = is >> ic & 1;
					
					yuv[0] += signal * wy[iw];
					yuv[1] += signal * ntscUPhase[ib & NTSC_PHASEMASK] * wc[iw];
					yuv[2] += signal * ntscVPhase[ib & NTSC_PHASEMASK] * wc[iw];
				}
			}
			
			applyDecoderMatrix(rgb, yuv, decoderMatrix);
			applyGainAndOffset(rgb, config->contrast, config->brightness);
			applyGainAndOffset(rgb, NTSC_COLORGAIN,
							   (ib < NTSC_APPLEII_CHUNKSIZE) ? NTSC_COLOROFFSET : 0.0);
			
			signalToPixel[is][ib] = NTSC_PACK(rgb[0], rgb[1], rgb[2]);
		}
	}
}

void ntscAppleIIInit(NTSCAppleIISignalToPixel signalToPixel, NTSCConfiguration *config)
{
	double w[NTSC_APPLEII_FILTER_N];
	double wy[NTSC_APPLEII_FILTER_N];
	double wc[NTSC_APPLEII_FILTER_N];
	double decoderMatrix[NTSC_DECODERMATRIX_SIZE];
	
	calculateChebyshevWindow(w, NTSC_APPLEII_FILTER_N,
							 NTSC_APPLEII_CHEBYSHEV_SIDELOBE_DB);
	
	calculateLanczosWindow(wy, NTSC_APPLEII_FILTER_N, config->lumaBandwidth);
	multiplyWindow(wy, w, NTSC_APPLEII_FILTER_N);
	
	calculateLanczosWindow(wc, NTSC_APPLEII_FILTER_N, config->chromaBandwidth);
	multiplyWindow(wc, w, NTSC_APPLEII_FILTER_N);
	
	copyDecoderMatrix(decoderMatrix, config->decoderMatrix);
	transformDecoderMatrix(decoderMatrix, config->saturation, config->hue);
	
	calculateAppleIISignalToPixel(signalToPixel, config, wy, wc, decoderMatrix);
}

#define NTSC_APPLEII_WRITEPIXEL(index)\
	value = c0[(index - 0) % 24] + \
		c1[(index - 8) % 24] + \
		c2[(index - 16) % 24];\
	NTSC_CLAMP(value);\
	*o++ = NTSC_UNPACK(value);

#define NTSC_APPLEII_PROCESSCHUNK(index)\
	c##index = signalToPixel[*i++];\
	NTSC_APPLEII_WRITEPIXEL(index * NTSC_APPLEII_CHUNKSIZE + 0);\
	NTSC_APPLEII_WRITEPIXEL(index * NTSC_APPLEII_CHUNKSIZE + 1);\
	NTSC_APPLEII_WRITEPIXEL(index * NTSC_APPLEII_CHUNKSIZE + 2);\
	NTSC_APPLEII_WRITEPIXEL(index * NTSC_APPLEII_CHUNKSIZE + 3);\
	NTSC_APPLEII_WRITEPIXEL(index * NTSC_APPLEII_CHUNKSIZE + 4);\
	NTSC_APPLEII_WRITEPIXEL(index * NTSC_APPLEII_CHUNKSIZE + 5);\
	NTSC_APPLEII_WRITEPIXEL(index * NTSC_APPLEII_CHUNKSIZE + 6);\
	NTSC_APPLEII_WRITEPIXEL(index * NTSC_APPLEII_CHUNKSIZE + 7);

void ntscAppleIIBlit(NTSCAppleIISignalToPixel signalToPixel,
					 int *output, unsigned char *input,
					 int width, int height)
{
	int blockNum = width / NTSC_APPLEII_BLOCKSIZE;
	int inputWidth = (width + 7) / 8;
	int outputWidth = width;
	
	while (height--)
	{
		int *c0;
		int *c1 = signalToPixel[NTSC_APPLEII_SIGNALBLACK];
		int *c2 = signalToPixel[NTSC_APPLEII_SIGNALBLACK];
		unsigned char *i = input;
		int *o = output;
		int value;
		int n;
		
		for (n = blockNum; n; n--)
		{
			NTSC_APPLEII_PROCESSCHUNK(0);
			NTSC_APPLEII_PROCESSCHUNK(1);
			NTSC_APPLEII_PROCESSCHUNK(2);
		}
		
		input += inputWidth;
		output += outputWidth;
	}
	
	return;
}
