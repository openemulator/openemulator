
/**
 * libntsc
 * CGA NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a CGA NTSC emulation.
 */

#ifndef _COMPOSITE_CGA_H
#define _COMPOSITE_CGA_H

// Parameters:
#define COMPOSITE_CGA_FILTER_N	17
#define COMPOSITE_CGA_CHEBYSHEV_SIDELOBE_DB	50.0

// Constants:
#define COMPOSITE_CGA_INPUTBITNUM		8
#define COMPOSITE_CGA_INPUTSIZE			(1 << COMPOSITE_CGA_INPUTBITNUM)
#define COMPOSITE_CGA_INPUTSAMPLENUM	4

#define COMPOSITE_CGA_OUTPUTSIZE		24

#define COMPOSITE_CGA_INPUTBLACK		0

typedef int CompositeCGA[COMPOSITE_CGA_INPUTSIZE][COMPOSITE_CGA_OUTPUTSIZE];

void compositeCGAInit(CompositeCGA convolutionTable,
					  CompositeConfiguration *config);
void compositeCGABlit(CompositeCGA convolutionTable,
					  unsigned char *input, int width, int height,
					  int *output, int outputPitch, int doubleScanlines);

#endif
