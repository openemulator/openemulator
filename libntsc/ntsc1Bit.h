
/**
 * libntsc
 * 1-bit NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a 1-bit NTSC emulation.
 */

#ifndef _NTSC_1BIT_H
#define _NTSC_1BIT_H

// Parameters:
#define NTSC_1BIT_FILTER_N	17
#define NTSC_1BIT_CHEBYSHEV_SIDELOBE_DB	50.0

// Constants:
#define NTSC_1BIT_INPUTBITNUM		8
#define NTSC_1BIT_INPUTSIZE			(1 << NTSC_1BIT_INPUTBITNUM)
#define NTSC_1BIT_INPUTSAMPLENUM	8

#define NTSC_1BIT_OUTPUTSIZE		24

#define NTSC_1BIT_INPUTBLACK		0

typedef int NTSC1Bit[NTSC_1BIT_INPUTSIZE][NTSC_1BIT_OUTPUTSIZE];

void ntsc1BitInit(NTSC1Bit convolutionTable,
				  NTSCConfiguration *config);
void ntsc1BitBlit(NTSC1Bit convolutionTable,
				  unsigned char *input, int width, int height,
				  int *output, int outputPitch, int doubleScanlines);

#endif
