
/**
 * libcomposite
 * CGA composite video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a CGA composite video emulation.
 */

#include "composite.h"
#include "compositeSignalProcessing.h"

#define COMPOSITE_CGA_RGBITOSAMPLEBITNUM	16
#define COMPOSITE_CGA_RGBITOSAMPLESIZE	(1 << COMPOSITE_CGA_RGBITOSAMPLEBITNUM)

#define COMPOSITE_CGA_COLORNUM	16

int compositeCGARGBiToSignal[COMPOSITE_CGA_COLORNUM][COMPOSITE_PHASENUM] =
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

int compositeCGAChunkToSignalCalculated = 0;
static unsigned char compositeCGAChunkToSignal[COMPOSITE_CGA_RGBITOSAMPLESIZE];

void calculateCGAChunkToSignal()
{
	if (compositeCGAChunkToSignalCalculated)
		return;
	
	for (int i = 0; i < COMPOSITE_CGA_RGBITOSAMPLESIZE; i++)
	{
		unsigned char index[2];
		int signal;
		
		index[0] = i >> 0 & 0xff;
		index[1] = i >> 8 & 0xff;
		signal = (compositeCGARGBiToSignal[i >> 0 & 0xf][0] << 0 |
				  compositeCGARGBiToSignal[i >> 4 & 0xf][1] << 2 |
				  compositeCGARGBiToSignal[i >> 8 & 0xf][2] << 4 |
				  compositeCGARGBiToSignal[i >> 12 & 0xf][3] << 6);
		
		compositeCGAChunkToSignal[*((unsigned short *) index)] = signal;
	}
	
	compositeCGAChunkToSignalCalculated = 1;
}

//
// RGB block convolution based on Blargg's NTSC NES emulation
//
void calculateCGASignalToPixel(CompositeCGA convolutionTable,
							   CompositeConfiguration *config,
							   float *wy, float *wc, float *decoderMatrix)
{
	for (int i = 0; i < COMPOSITE_CGA_INPUTSIZE; i++)
	{
		for (int o = 0; o < COMPOSITE_CGA_OUTPUTSIZE; o++)
		{
			float yuv[3] = {0.0, 0.0, 0.0};
			float rgb[3];
			
			// Convolution
			for (int c = 0; c < COMPOSITE_CGA_INPUTSAMPLENUM; c++)
			{
				int w = o - c;
				if ((w >= 0) && (w < COMPOSITE_CGA_FILTER_N))
				{
					// Calculate composite signal level
					float signal = (i >> (c << 1) & 3) * (1.0 / 3.0);
					
					yuv[0] += signal * wy[w];
					yuv[1] += signal * compositeUPhase[(c + 2) & COMPOSITE_PHASEMASK] * wc[w];
					yuv[2] += signal * compositeVPhase[(c + 2) & COMPOSITE_PHASEMASK] * wc[w];
				}
			}
			
			if (config->colorize)
			{
				yuv[1] = (1.0 / M_SQRT2) * yuv[0];
				yuv[2] = 0;
			}
			
			applyDecoderMatrix(rgb, yuv, decoderMatrix);
			applyOffsetAndGain(rgb, o < COMPOSITE_CGA_INPUTSAMPLENUM ?
							   config->brightness : 0, config->contrast);
			applyOffsetAndGain(rgb, o < COMPOSITE_CGA_INPUTSAMPLENUM ?
							   COMPOSITE_COLOROFFSET : 0, COMPOSITE_COLORGAIN);
			
			convolutionTable[i][o] = COMPOSITE_PACK(rgb[0], rgb[1], rgb[2]);
		}
	}
}

void compositeCGAInit(CompositeCGA convolutionTable,
					  CompositeConfiguration *config)
{
	float w[COMPOSITE_CGA_FILTER_N];
	float wy[COMPOSITE_CGA_FILTER_N];
	float wc[COMPOSITE_CGA_FILTER_N];
	float decoderMatrix[COMPOSITE_DECODERMATRIX_SIZE];
	
	calculateChebyshevWindow(w, COMPOSITE_CGA_FILTER_N,
							 COMPOSITE_CGA_CHEBYSHEV_SIDELOBE_DB);
	
	calculateLanczosWindow(wy, COMPOSITE_CGA_FILTER_N, config->lumaBandwidth);
	multiplyWindow(wy, w, COMPOSITE_CGA_FILTER_N);
	normalizeWindow(wy, COMPOSITE_CGA_FILTER_N);
	
	calculateLanczosWindow(wc, COMPOSITE_CGA_FILTER_N, config->chromaBandwidth);
	multiplyWindow(wc, w, COMPOSITE_CGA_FILTER_N);
	normalizeWindow(wc, COMPOSITE_CGA_FILTER_N);
	
	copyDecoderMatrix(decoderMatrix, config->decoderMatrix);
	transformDecoderMatrix(decoderMatrix, config->saturation, config->hue);
	
	calculateCGAChunkToSignal();
	calculateCGASignalToPixel(convolutionTable, config, wy, wc, decoderMatrix);
}

#define COMPOSITE_CGA_WRITEPIXEL(index)\
	value = c0[(index + 24 - 0) % 24] + \
		c1[(index + 24 - 4) % 24] + \
		c2[(index + 24 - 8) % 24] + \
		c3[(index + 24 - 12) % 24] + \
		c4[(index + 24 - 16) % 24] + \
		c5[(index + 24 - 20) % 24];\
	COMPOSITE_CLAMP(value);\
	*o++ = COMPOSITE_UNPACK(value);

#define COMPOSITE_CGA_PROCESSCHUNK(index)\
	c##index = convolutionTable[compositeCGAChunkToSignal[*i++]];\
	COMPOSITE_CGA_WRITEPIXEL(index * COMPOSITE_CGA_INPUTSAMPLENUM + 0);\
	COMPOSITE_CGA_WRITEPIXEL(index * COMPOSITE_CGA_INPUTSAMPLENUM + 1);\
	COMPOSITE_CGA_WRITEPIXEL(index * COMPOSITE_CGA_INPUTSAMPLENUM + 2);\
	COMPOSITE_CGA_WRITEPIXEL(index * COMPOSITE_CGA_INPUTSAMPLENUM + 3);

void compositeCGABlit(CompositeCGA convolutionTable,
					  unsigned char *input, int width, int height,
					  int *output, int outputPitch)
{
	int inputWidth = (width + 1) / 2;
	int inputHeight = height;
	
	int outputNum = width / COMPOSITE_CGA_OUTPUTSIZE;
	int outputWidth = width + outputPitch;
	int *frameBuffer = output;
	
	while (inputHeight--)
	{
		int *c0;
		int *c1 = convolutionTable[COMPOSITE_CGA_INPUTBLACK];
		int *c2 = convolutionTable[COMPOSITE_CGA_INPUTBLACK];
		int *c3 = convolutionTable[COMPOSITE_CGA_INPUTBLACK];
		int *c4 = convolutionTable[COMPOSITE_CGA_INPUTBLACK];
		int *c5 = convolutionTable[COMPOSITE_CGA_INPUTBLACK];
		
		unsigned short *i = (unsigned short *) input;
		int *o = output;
		int value;
		
		for (int n = outputNum; n; n--)
		{
			COMPOSITE_CGA_PROCESSCHUNK(0);
			COMPOSITE_CGA_PROCESSCHUNK(1);
			COMPOSITE_CGA_PROCESSCHUNK(2);
			COMPOSITE_CGA_PROCESSCHUNK(3);
			COMPOSITE_CGA_PROCESSCHUNK(4);
			COMPOSITE_CGA_PROCESSCHUNK(5);
		}
		
		input += inputWidth;
		output += outputWidth;
	}
	
	return;
}
