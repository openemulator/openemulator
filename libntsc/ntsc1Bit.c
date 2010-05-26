
/**
 * libntsc
 * 1-bit NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a 1-bit NTSC emulation.
 */

#include "ntsc.h"
#include "ntscSignalProcessing.h"

//
// RGB block convolution based on Blargg's NTSC NES emulation
//
void calculate1BitSignalToPixel(NTSC1Bit convolutionTable,
								NTSCConfiguration *config,
								float *wy, float *wc, float *decoderMatrix)
{
	for (int i = 0; i < NTSC_1BIT_INPUTSIZE; i++)
	{
		for (int o = 0; o < NTSC_1BIT_OUTPUTSIZE; o++)
		{
			float yuv[3] = {0.0, 0.0, 0.0};
			float rgb[3];
			
			// Convolution
			for (int c = 0; c < NTSC_1BIT_INPUTSAMPLENUM; c++)
			{
				int w = o - c;
				if ((w >= 0) && (w < NTSC_1BIT_FILTER_N))
				{
					// Calculate composite signal level
					float signal = i >> c & 1;
					
					yuv[0] += signal * wy[w];
					yuv[1] += signal * ntscUPhase[c & NTSC_PHASEMASK] * wc[w];
					yuv[2] += signal * ntscVPhase[c & NTSC_PHASEMASK] * wc[w];
				}
			}
			
			if (config->colorize)
			{
				yuv[1] = (1.0 / M_SQRT2) * yuv[0];
				yuv[2] = 0;
			}
			
			applyDecoderMatrix(rgb, yuv, decoderMatrix);
			applyOffsetAndGain(rgb, o < NTSC_1BIT_INPUTSAMPLENUM ?
							   config->brightness : 0, config->contrast);
			applyOffsetAndGain(rgb, o < NTSC_1BIT_INPUTSAMPLENUM ?
							   NTSC_COLOROFFSET : 0, NTSC_COLORGAIN);
			
			convolutionTable[i][o] = NTSC_PACK(rgb[0], rgb[1], rgb[2]);
		}
	}
}

void ntsc1BitInit(NTSC1Bit convolutionTable, NTSCConfiguration *config)
{
	float w[NTSC_1BIT_FILTER_N];
	float wy[NTSC_1BIT_FILTER_N];
	float wc[NTSC_1BIT_FILTER_N];
	float decoderMatrix[NTSC_DECODERMATRIX_SIZE];
	
	calculateChebyshevWindow(w, NTSC_1BIT_FILTER_N,
							 NTSC_1BIT_CHEBYSHEV_SIDELOBE_DB);
	
	calculateLanczosWindow(wy, NTSC_1BIT_FILTER_N, config->lumaBandwidth);
	multiplyWindow(wy, w, NTSC_1BIT_FILTER_N);
	normalizeWindow(wy, NTSC_1BIT_FILTER_N);
	
	calculateLanczosWindow(wc, NTSC_1BIT_FILTER_N, config->chromaBandwidth);
	multiplyWindow(wc, w, NTSC_1BIT_FILTER_N);
	normalizeWindow(wc, NTSC_1BIT_FILTER_N);
	
	copyDecoderMatrix(decoderMatrix, config->decoderMatrix);
	transformDecoderMatrix(decoderMatrix, config->saturation, config->hue);
	
	calculate1BitSignalToPixel(convolutionTable, config, wy, wc, decoderMatrix);
}

#define NTSC_1BIT_WRITEPIXEL(index)\
	value = c0[(index + 24 - 0) % 24] +\
	c1[(index + 24 - 8) % 24] +\
	c2[(index + 24 - 16) % 24];\
	NTSC_CLAMP(value);\
	*o++ = NTSC_UNPACK(value);

#define NTSC_1BIT_PROCESSCHUNK(index)\
	c##index = convolutionTable[*i++];\
	NTSC_1BIT_WRITEPIXEL(index * NTSC_1BIT_INPUTSAMPLENUM + 0);\
	NTSC_1BIT_WRITEPIXEL(index * NTSC_1BIT_INPUTSAMPLENUM + 1);\
	NTSC_1BIT_WRITEPIXEL(index * NTSC_1BIT_INPUTSAMPLENUM + 2);\
	NTSC_1BIT_WRITEPIXEL(index * NTSC_1BIT_INPUTSAMPLENUM + 3);\
	NTSC_1BIT_WRITEPIXEL(index * NTSC_1BIT_INPUTSAMPLENUM + 4);\
	NTSC_1BIT_WRITEPIXEL(index * NTSC_1BIT_INPUTSAMPLENUM + 5);\
	NTSC_1BIT_WRITEPIXEL(index * NTSC_1BIT_INPUTSAMPLENUM + 6);\
	NTSC_1BIT_WRITEPIXEL(index * NTSC_1BIT_INPUTSAMPLENUM + 7);

void ntsc1BitBlit(NTSC1Bit convolutionTable,
				  unsigned char *input, int width, int height,
				  int *output, int outputPitch, int doubleScanlines)
{
	int inputWidth = (width + 7) / 8;
	int inputHeight = height;
	
	int outputNum = width / NTSC_1BIT_OUTPUTSIZE;
	int outputWidth = doubleScanlines ? (width + outputPitch) * 2 : width + outputPitch;
	int *frameBuffer = output;
	
	while (inputHeight--)
	{
		int *c0;
		int *c1 = convolutionTable[NTSC_1BIT_INPUTBLACK];
		int *c2 = convolutionTable[NTSC_1BIT_INPUTBLACK];
		
		unsigned char *i = input;
		int *o = output;
		int value;
		
		for (int n = outputNum; n; n--)
		{
			NTSC_1BIT_PROCESSCHUNK(0);
			NTSC_1BIT_PROCESSCHUNK(1);
			NTSC_1BIT_PROCESSCHUNK(2);
		}
		
		input += inputWidth;
		output += outputWidth;
	}
	
	if (doubleScanlines)
		interpolateScanlines(frameBuffer, width, height, outputPitch);
	
	return;
}
