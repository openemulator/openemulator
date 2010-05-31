
/**
 * libcomposite
 * 1-bit composite video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a 1-bit composite video emulation.
 */

#include "composite.h"
#include "compositeSignalProcessing.h"

//
// RGB block convolution based on Blargg's NTSC NES emulation
//
void calculate1BitSignalToPixel(Composite1Bit convolutionTable,
								CompositeConfiguration *config,
								float *wy, float *wc, float *decoderMatrix)
{
	for (int i = 0; i < COMPOSITE_1BIT_INPUTSIZE; i++)
	{
		for (int o = 0; o < COMPOSITE_1BIT_OUTPUTSIZE; o++)
		{
			float yuv[3] = {0.0, 0.0, 0.0};
			float rgb[3];
			
			// Convolution
			for (int c = 0; c < COMPOSITE_1BIT_INPUTSAMPLENUM; c++)
			{
				int w = o - c;
				if ((w >= 0) && (w < COMPOSITE_1BIT_FILTER_N))
				{
					// Calculate composite signal level
					float signal = i >> c & 1;
					
					yuv[0] += signal * wy[w];
					yuv[1] += signal * compositeUPhase[c & COMPOSITE_PHASEMASK] * wc[w];
					yuv[2] += signal * compositeVPhase[c & COMPOSITE_PHASEMASK] * wc[w];
				}
			}
			
			if (config->colorize)
			{
				yuv[1] = (1.0 / M_SQRT2) * yuv[0];
				yuv[2] = 0;
			}
			
			applyDecoderMatrix(rgb, yuv, decoderMatrix);
			applyOffsetAndGain(rgb, o < COMPOSITE_1BIT_INPUTSAMPLENUM ?
							   config->brightness : 0, config->contrast);
			applyOffsetAndGain(rgb, o < COMPOSITE_1BIT_INPUTSAMPLENUM ?
							   COMPOSITE_COLOROFFSET : 0, COMPOSITE_COLORGAIN);
			
			convolutionTable[i][o] = COMPOSITE_PACK(rgb[0], rgb[1], rgb[2]);
		}
	}
}

void composite1BitInit(Composite1Bit convolutionTable,
					   CompositeConfiguration *config)
{
	float w[COMPOSITE_1BIT_FILTER_N];
	float wy[COMPOSITE_1BIT_FILTER_N];
	float wc[COMPOSITE_1BIT_FILTER_N];
	float decoderMatrix[COMPOSITE_DECODERMATRIX_SIZE];
	
	calculateChebyshevWindow(w, COMPOSITE_1BIT_FILTER_N,
							 COMPOSITE_1BIT_CHEBYSHEV_SIDELOBE_DB);
	
	calculateLanczosWindow(wy, COMPOSITE_1BIT_FILTER_N, config->lumaBandwidth);
	multiplyWindow(wy, w, COMPOSITE_1BIT_FILTER_N);
	normalizeWindow(wy, COMPOSITE_1BIT_FILTER_N);
	
	calculateLanczosWindow(wc, COMPOSITE_1BIT_FILTER_N, config->chromaBandwidth);
	multiplyWindow(wc, w, COMPOSITE_1BIT_FILTER_N);
	normalizeWindow(wc, COMPOSITE_1BIT_FILTER_N);
	
	copyDecoderMatrix(decoderMatrix, config->decoderMatrix);
	transformDecoderMatrix(decoderMatrix, config->saturation, config->hue);
	
	calculate1BitSignalToPixel(convolutionTable, config, wy, wc, decoderMatrix);
}

#define COMPOSITE_1BIT_WRITEPIXEL(index)\
	value = c0[(index + 24 - 0) % 24] +\
	c1[(index + 24 - 8) % 24] +\
	c2[(index + 24 - 16) % 24];\
	COMPOSITE_CLAMP(value);\
	*o++ = COMPOSITE_UNPACK(value);

#define COMPOSITE_1BIT_PROCESSCHUNK(index)\
	c##index = convolutionTable[*i++];\
	COMPOSITE_1BIT_WRITEPIXEL(index * COMPOSITE_1BIT_INPUTSAMPLENUM + 0);\
	COMPOSITE_1BIT_WRITEPIXEL(index * COMPOSITE_1BIT_INPUTSAMPLENUM + 1);\
	COMPOSITE_1BIT_WRITEPIXEL(index * COMPOSITE_1BIT_INPUTSAMPLENUM + 2);\
	COMPOSITE_1BIT_WRITEPIXEL(index * COMPOSITE_1BIT_INPUTSAMPLENUM + 3);\
	COMPOSITE_1BIT_WRITEPIXEL(index * COMPOSITE_1BIT_INPUTSAMPLENUM + 4);\
	COMPOSITE_1BIT_WRITEPIXEL(index * COMPOSITE_1BIT_INPUTSAMPLENUM + 5);\
	COMPOSITE_1BIT_WRITEPIXEL(index * COMPOSITE_1BIT_INPUTSAMPLENUM + 6);\
	COMPOSITE_1BIT_WRITEPIXEL(index * COMPOSITE_1BIT_INPUTSAMPLENUM + 7);

void composite1BitBlit(Composite1Bit convolutionTable,
					   unsigned char *input, int width, int height,
					   int *output, int outputPitch, int doubleScanlines)
{
	int inputWidth = (width + 7) / 8;
	int inputHeight = height;
	
	int outputNum = width / COMPOSITE_1BIT_OUTPUTSIZE;
	int outputWidth = doubleScanlines ? (width + outputPitch) * 2 : width + outputPitch;
	int *frameBuffer = output;
	
	while (inputHeight--)
	{
		int *c0;
		int *c1 = convolutionTable[COMPOSITE_1BIT_INPUTBLACK];
		int *c2 = convolutionTable[COMPOSITE_1BIT_INPUTBLACK];
		
		unsigned char *i = input;
		int *o = output;
		int value;
		
		for (int n = outputNum; n; n--)
		{
			COMPOSITE_1BIT_PROCESSCHUNK(0);
			COMPOSITE_1BIT_PROCESSCHUNK(1);
			COMPOSITE_1BIT_PROCESSCHUNK(2);
		}
		
		input += inputWidth;
		output += outputWidth;
	}
	
	if (doubleScanlines)
		interpolateScanlines(frameBuffer, width, height, outputPitch);
	
	return;
}
