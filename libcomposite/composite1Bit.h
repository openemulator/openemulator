
/**
 * libntsc
 * 1-bit NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a 1-bit NTSC emulation.
 */

#ifndef _COMPOSITE_1BIT_H
#define _COMPOSITE_1BIT_H

// Parameters:
#define COMPOSITE_1BIT_FILTER_N	17
#define COMPOSITE_1BIT_CHEBYSHEV_SIDELOBE_DB	50.0

// Constants:
#define COMPOSITE_1BIT_INPUTBITNUM		8
#define COMPOSITE_1BIT_INPUTSIZE		(1 << COMPOSITE_1BIT_INPUTBITNUM)
#define COMPOSITE_1BIT_INPUTSAMPLENUM	8

#define COMPOSITE_1BIT_OUTPUTSIZE		24

#define COMPOSITE_1BIT_INPUTBLACK		0

typedef int Composite1Bit[COMPOSITE_1BIT_INPUTSIZE][COMPOSITE_1BIT_OUTPUTSIZE];

void composite1BitInit(Composite1Bit convolutionTable,
					   CompositeConfiguration *config);
void composite1BitBlit(Composite1Bit convolutionTable,
					   unsigned char *input, int width, int height,
					   int *output, int outputPitch, int doubleScanlines);

#endif
