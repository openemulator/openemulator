
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

// Filter size should be a multiple of 8, minus 1.
#define NTSC_CGA_FILTER_N	17
#define NTSC_CGA_CHEBYSHEV_SIDELOBE_DB	50.0

// Do not modify these defines:
#define NTSC_CGA_CHUNKSIZE	8
#define NTSC_CGA_CHUNKNUM	(1 << NTSC_APPLEII_CHUNKSIZE)
#define NTSC_CGA_BLOCKSIZE	(NTSC_APPLEII_FILTER_N + NTSC_APPLEII_CHUNKSIZE - 1)
#define NTSC_CGA_BLOCKBYTESIZE (NTSC_APPLEII_BLOCKSIZE / 8)

#define NTSC_CGA_BLACK 0

typedef int NTSCCGAData[NTSC_CGA_CHUNKNUM][NTSC_CGA_BLOCKSIZE];

int ntscCGAInit(NTSCCGAData data, NTSCConfiguration *config);
int ntscCGABlit(NTSCCGAData data,
				int *output, int *input,
				int width, int height);

#endif
