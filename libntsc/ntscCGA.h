
/**
 * libntsc
 * CGA NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a CGA NTSC emulation.
 */

#ifndef _NTSC_CGA_H
#define _NTSC_CGA_H

// Modifiable parameters:
#define NTSC_CGA_FILTER_N	17
#define NTSC_CGA_CHEBYSHEV_SIDELOBE_DB	50.0

// Do not modify these defines:
#define NTSC_CGA_INPUTNUM		16
#define NTSC_CGA_INPUTSIZE		(1 << NTSC_CGA_INPUTNUM)
#define NTSC_CGA_SIGNALNUM		8
#define NTSC_CGA_SIGNALSIZE		(1 << NTSC_CGA_SIGNALNUM)
#define NTSC_CGA_BLOCKSIZE		24
#define NTSC_CGA_CHUNKSIZE		4
#define NTSC_CGA_OFFSETGAIN		(1.0 * NTSC_CGA_CHUNKSIZE / NTSC_CGA_BLOCKSIZE)

#define NTSC_CGA_SIGNALBLACK 0

typedef int NTSCCGASignalToPixel[NTSC_CGA_SIGNALSIZE][NTSC_CGA_BLOCKSIZE];

void ntscCGAInit(NTSCCGASignalToPixel signalToPixel,
				NTSCConfiguration *config);
void ntscCGABlit(NTSCCGASignalToPixel signalToPixel,
				int *output, unsigned char *input,
				int width, int height);

#endif
