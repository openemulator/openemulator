
/**
 * libntsc
 * 2-Bit NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a 2-bit NTSC emulation.
 */

#ifndef _NTSC_2BIT_H
#define _NTSC_2BIT_H

// Parameters:
#define NTSC_2BIT_FILTER_N	17
#define NTSC_2BIT_CHEBYSHEV_SIDELOBE_DB	50.0

// Constants:
#define NTSC_2BIT_INPUTBITNUM		8
#define NTSC_2BIT_INPUTSIZE			(1 << NTSC_2BIT_INPUTBITNUM)
#define NTSC_2BIT_INPUTSAMPLENUM	4

#define NTSC_2BIT_OUTPUTSIZE		24

#define NTSC_2BIT_INPUTBLACK		0

typedef int NTSC2Bit[NTSC_2BIT_INPUTSIZE][NTSC_2BIT_OUTPUTSIZE];

void ntsc2BitInit(NTSC2Bit convolutionTable,
				  NTSCConfiguration *config);
void ntsc2BitBlit(NTSC2Bit convolutionTable,
				  unsigned char *input, int width, int height,
				  int *output, int outputPitch, int doubleScanlines);

#endif
