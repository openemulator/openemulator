
/**
 * libntsc
 * 2-bit NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a 2-bit NTSC emulation (CGA).
 */

#include "ntsc.h"
#include "ntscSignalProcessing.h"

#define NTSC_CGA_RGBITOSAMPLEBITNUM	16
#define NTSC_CGA_RGBITOSAMPLESIZE	(1 << NTSC_CGA_RGBITOSAMPLEBITNUM)

#define NTSC_CGA_COLORNUM	16

int ntscCGARGBiToSignal[NTSC_CGA_COLORNUM][NTSC_PHASENUM] =
{
	{0, 0, 0, 0},
	{0, 1, 2, 1},
	{2, 0, 0, 2},
	{1, 0, 1, 2},
	{1, 2, 1, 0},
	{0, 2, 2, 0},
	{2, 1, 0, 1},
	{2, 2, 2, 2},
	
	{1, 1, 1, 1},
	{1, 2, 3, 2},
	{3, 1, 1, 3},
	{2, 1, 2, 3},
	{2, 3, 2, 1},
	{1, 3, 3, 1},
	{3, 2, 1, 2},
	{3, 3, 3, 3},
};

int ntscCGAChunkToSignalCalculated = 0;
static unsigned char ntscCGAChunkToSignal[NTSC_CGA_RGBITOSAMPLESIZE];

void calculateCGAChunkToSignal()
{
	if (ntscCGAChunkToSignalCalculated)
		return;
	
	for (int i = 0; i < NTSC_CGA_RGBITOSAMPLESIZE; i++)
	{
		unsigned char index[2];
		int signal;
		
		index[0] = i >> 0 & 0xff;
		index[1] = i >> 8 & 0xff;
		signal = (ntscCGARGBiToSignal[i >> 0 & 0xf][0] << 0 |
				  ntscCGARGBiToSignal[i >> 4 & 0xf][1] << 2 |
				  ntscCGARGBiToSignal[i >> 8 & 0xf][2] << 4 |
				  ntscCGARGBiToSignal[i >> 12 & 0xf][3] << 6);
		
		ntscCGAChunkToSignal[*((unsigned short *) index)] = signal;
	}
	
	ntscCGAChunkToSignalCalculated = 1;
}

//
// RGB block convolution based on Blargg's NTSC NES emulation
//
void calculateCGASignalToPixel(NTSCCGA convolutionTable,
							   NTSCConfiguration *config,
							   float *wy, float *wc, float *decoderMatrix)
{
	for (int i = 0; i < NTSC_CGA_INPUTSIZE; i++)
	{
		for (int o = 0; o < NTSC_CGA_OUTPUTSIZE; o++)
		{
			float yuv[3] = {0.0, 0.0, 0.0};
			float rgb[3];
			
			// Convolution
			for (int c = 0; c < NTSC_CGA_INPUTSAMPLENUM; c++)
			{
				int w = o - c;
				if ((w >= 0) && (w < NTSC_CGA_FILTER_N))
				{
					// Calculate composite signal level
					float signal = (i >> (c << 1) & 3) * (1.0 / 3.0);
					
					yuv[0] += signal * wy[w];
					yuv[1] += signal * ntscUPhase[(c + 2) & NTSC_PHASEMASK] * wc[w];
					yuv[2] += signal * ntscVPhase[(c + 2) & NTSC_PHASEMASK] * wc[w];
				}
			}
			
			if (config->colorize)
			{
				yuv[1] = (1.0 / M_SQRT2) * yuv[0];
				yuv[2] = 0;
			}
			
			applyDecoderMatrix(rgb, yuv, decoderMatrix);
			applyOffsetAndGain(rgb, o < NTSC_CGA_INPUTSAMPLENUM ?
							   config->brightness : 0, config->contrast);
			applyOffsetAndGain(rgb, o < NTSC_CGA_INPUTSAMPLENUM ?
							   NTSC_COLOROFFSET : 0, NTSC_COLORGAIN);
			
			convolutionTable[i][o] = NTSC_PACK(rgb[0], rgb[1], rgb[2]);
		}
	}
}

void ntscCGAInit(NTSCCGA convolutionTable,
				 NTSCConfiguration *config)
{
	float w[NTSC_CGA_FILTER_N];
	float wy[NTSC_CGA_FILTER_N];
	float wc[NTSC_CGA_FILTER_N];
	float decoderMatrix[NTSC_DECODERMATRIX_SIZE];
	
	calculateChebyshevWindow(w, NTSC_CGA_FILTER_N,
							 NTSC_CGA_CHEBYSHEV_SIDELOBE_DB);
	
	calculateLanczosWindow(wy, NTSC_CGA_FILTER_N, config->lumaBandwidth);
	multiplyWindow(wy, w, NTSC_CGA_FILTER_N);
	normalizeWindow(wy, NTSC_CGA_FILTER_N);
	
	calculateLanczosWindow(wc, NTSC_CGA_FILTER_N, config->chromaBandwidth);
	multiplyWindow(wc, w, NTSC_CGA_FILTER_N);
	normalizeWindow(wc, NTSC_CGA_FILTER_N);
	
	copyDecoderMatrix(decoderMatrix, config->decoderMatrix);
	transformDecoderMatrix(decoderMatrix, config->saturation, config->hue);
	
	calculateCGAChunkToSignal();
	calculateCGASignalToPixel(convolutionTable, config, wy, wc, decoderMatrix);
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
	c##index = convolutionTable[ntscCGAChunkToSignal[*i++]];\
	NTSC_CGA_WRITEPIXEL(index * NTSC_CGA_INPUTSAMPLENUM + 0);\
	NTSC_CGA_WRITEPIXEL(index * NTSC_CGA_INPUTSAMPLENUM + 1);\
	NTSC_CGA_WRITEPIXEL(index * NTSC_CGA_INPUTSAMPLENUM + 2);\
	NTSC_CGA_WRITEPIXEL(index * NTSC_CGA_INPUTSAMPLENUM + 3);

void ntscCGABlit(NTSCCGA convolutionTable,
				 unsigned char *input, int width, int height,
				 int *output, int outputPitch, int doubleScanlines)
{
	int inputWidth = (width + 1) / 2;
	int inputHeight = height;
	
	int outputNum = width / NTSC_CGA_OUTPUTSIZE;
	int outputWidth = doubleScanlines ? (width + outputPitch) * 2 : width + outputPitch;
	int *frameBuffer = output;
	
	while (inputHeight--)
	{
		int *c0;
		int *c1 = convolutionTable[NTSC_CGA_INPUTBLACK];
		int *c2 = convolutionTable[NTSC_CGA_INPUTBLACK];
		int *c3 = convolutionTable[NTSC_CGA_INPUTBLACK];
		int *c4 = convolutionTable[NTSC_CGA_INPUTBLACK];
		int *c5 = convolutionTable[NTSC_CGA_INPUTBLACK];
		
		unsigned short *i = (unsigned short *) input;
		int *o = output;
		int value;
		
		for (int n = outputNum; n; n--)
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
	
	if (doubleScanlines)
		interpolateScanlines(frameBuffer, width, height, outputPitch);
	
	return;
}
