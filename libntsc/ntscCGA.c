
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

int ntscCGARGBiToSignal[NTSC_CGA_COLORNUM][NTSC_PHASENUM] =
{
	{0, 0, 0, 0},
	{0, 0, 2, 2},
	{2, 1, 0, 1},
	{2, 0, 0, 2},
	{0, 2, 2, 0},
	{0, 1, 2, 1},
	{2, 2, 0, 0},
	{2, 2, 2, 2},
	
	{1, 1, 1, 1},
	{1, 1, 3, 3},
	{3, 2, 1, 2},
	{3, 1, 1, 3},
	{1, 3, 3, 1},
	{1, 2, 3, 2},
	{3, 3, 1, 1},
	{3, 3, 3, 3},
};

int ntscCGAChunkToSignalCalculated = 0;
static unsigned char ntscCGAChunkToSignal[NTSC_CGA_INPUTSIZE];

void calculateCGAChunkToSignal()
{
	if (ntscCGAChunkToSignalCalculated)
		return;
	
	for (int i = 0; i < NTSC_CGA_INPUTSIZE; i++)
	{
		unsigned char index[2];
		int signal;
		
		index[0] = i >> 0 & 0xff;
		index[1] = i >> 8 & 0xff;
		signal = (ntscCGARGBiToSignal[i >> 0 & 0xf][0] << 0 |
				  ntscCGARGBiToSignal[i >> 4 & 0xf][1] << 2 |
				  ntscCGARGBiToSignal[i >> 8 & 0xf][2] << 4 |
				  ntscCGARGBiToSignal[i >> 2 & 0xf][3] << 6);
		
		ntscCGAChunkToSignal[*((unsigned short *) index)] = signal;
	}
	
	ntscCGAChunkToSignalCalculated = 1;
}

void calculateCGASignalToPixel(NTSCCGASignalToPixel signalToPixel,
							   NTSCConfiguration *config,
							   double *wy, double *wc, double *decoderMatrix)
{
	for (int is = 0; is < NTSC_CGA_SIGNALSIZE; is++)
	{
		for (int ib = 0; ib < NTSC_CGA_BLOCKSIZE; ib++)
		{
			double yuv[3] = {0.0, 0.0, 0.0};
			double rgb[3];
			
			// Convolution
			for (int ic = 0; ic < NTSC_CGA_CHUNKSIZE; ic++)
			{
				int iw = ib - ib;
				if ((iw >= 0) && (iw < NTSC_CGA_FILTER_N))
				{
					// Calculate composite signal level
					double signal = (is >> (ic << 1) & 3) * (1.0 / 3.0);
					
					yuv[0] += signal * wy[iw];
					yuv[1] += signal * ntscUPhase[ic & NTSC_PHASEMASK] * wc[iw];
					yuv[2] += signal * ntscVPhase[ic & NTSC_PHASEMASK] * wc[iw];
				}
			}
			
			applyDecoderMatrix(rgb, yuv, decoderMatrix);
			applyOffsetAndGain(rgb, config->brightness * NTSC_CGA_OFFSETGAIN,
							   config->contrast);
			applyOffsetAndGain(rgb, NTSC_COLOROFFSET * NTSC_CGA_OFFSETGAIN,
							   NTSC_COLORGAIN);
			
			signalToPixel[is][ib] = NTSC_PACK(rgb[0], rgb[1], rgb[2]);
		}
	}
}

void ntscCGAInit(NTSCCGASignalToPixel signalToPixel,
				NTSCConfiguration *config)
{
	double w[NTSC_CGA_FILTER_N];
	double wy[NTSC_CGA_FILTER_N];
	double wc[NTSC_CGA_FILTER_N];
	double decoderMatrix[NTSC_DECODERMATRIX_SIZE];
	
	calculateChebyshevWindow(w, NTSC_CGA_FILTER_N,
							 NTSC_CGA_CHEBYSHEV_SIDELOBE_DB);
	
	calculateLanczosWindow(wy, NTSC_CGA_FILTER_N, config->lumaBandwidth);
	multiplyWindow(wy, w, NTSC_CGA_FILTER_N);
	normalizeWindow(wy, NTSC_APPLEII_FILTER_N);
	
	calculateLanczosWindow(wc, NTSC_CGA_FILTER_N, config->chromaBandwidth);
	multiplyWindow(wc, w, NTSC_CGA_FILTER_N);
	normalizeWindow(wc, NTSC_APPLEII_FILTER_N);
	
	copyDecoderMatrix(decoderMatrix, config->decoderMatrix);
	transformDecoderMatrix(decoderMatrix, config->saturation, config->hue);
	
	calculateCGAChunkToSignal();
	calculateCGASignalToPixel(signalToPixel, config, wy, wc, decoderMatrix);
}

#define NTSC_CGA_WRITEPIXEL(index)\
	value = c0[(index + 24 - 0) % 24] + \
		c1[(index + 24 - 4) % 24] + \
		c2[(index + 24 - 8) % 24] + \
		c3[(index + 24 - 12) % 24] + \
		c4[(index + 24 - 16) % 24] + \
		c5[(index + 24 - 20) % 24];\
	NTSC_CLAMP(value);\
	*o++ = NTSC_UNPACK(value);

#define NTSC_CGA_PROCESSCHUNK(index)\
	c##index = signalToPixel[ntscCGAChunkToSignal[*i++]];\
	NTSC_CGA_WRITEPIXEL(index * NTSC_CGA_CHUNKSIZE + 0);\
	NTSC_CGA_WRITEPIXEL(index * NTSC_CGA_CHUNKSIZE + 1);\
	NTSC_CGA_WRITEPIXEL(index * NTSC_CGA_CHUNKSIZE + 2);\
	NTSC_CGA_WRITEPIXEL(index * NTSC_CGA_CHUNKSIZE + 3);

void ntscCGABlit(NTSCCGASignalToPixel signalToPixel,
				 int *output, unsigned char *input,
				 int width, int height)
{
	int blockNum = width / NTSC_CGA_BLOCKSIZE;
	int inputWidth = (width + 1) / 2;
	int outputWidth = width;
	
	while (height--)
	{
		int *c0;
		int *c1 = signalToPixel[NTSC_CGA_SIGNALBLACK];
		int *c2 = signalToPixel[NTSC_CGA_SIGNALBLACK];
		int *c3 = signalToPixel[NTSC_CGA_SIGNALBLACK];
		int *c4 = signalToPixel[NTSC_CGA_SIGNALBLACK];
		int *c5 = signalToPixel[NTSC_CGA_SIGNALBLACK];
		unsigned short *i = (unsigned short *) input;
		int *o = output;
		int value;
		int n;
		
		for (n = blockNum; n; n--)
		{
			NTSC_CGA_PROCESSCHUNK(0);
			NTSC_CGA_PROCESSCHUNK(1);
			NTSC_CGA_PROCESSCHUNK(2);
			NTSC_CGA_PROCESSCHUNK(3);
			NTSC_CGA_PROCESSCHUNK(4);
			NTSC_CGA_PROCESSCHUNK(5);
		}
		
		input += inputWidth;
		output += outputWidth;
	}
	
	return;
}
