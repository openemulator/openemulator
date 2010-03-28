
/**
 * libntsc
 * Apple II NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple II NTSC emulation.
 */

#ifndef _NTSC_APPLEII_H
#define _NTSC_APPLEII_H

// Filter size should be a multiple of 8, minus 1.
#define NTSC_APPLEII_FILTER_N	17
#define NTSC_APPLEII_CHEBYSHEV_SIDELOBE_DB	50.0

// Do not modify these defines:
#define NTSC_APPLEII_CHUNKSIZE	8
#define NTSC_APPLEII_CHUNKNUM	(1 << NTSC_APPLEII_CHUNKSIZE)
#define NTSC_APPLEII_BLOCKSIZE	(NTSC_APPLEII_FILTER_N + NTSC_APPLEII_CHUNKSIZE - 1)
#define NTSC_APPLEII_BLOCKBYTESIZE (NTSC_APPLEII_BLOCKSIZE / 8)

#define NTSC_APPLEII_BLACK 0

typedef int NTSCAppleIIData[NTSC_APPLEII_CHUNKNUM][NTSC_APPLEII_BLOCKSIZE];

int ntscAppleIIInit(NTSCAppleIIData data, NTSCConfiguration *config);
int ntscAppleIIBlit(NTSCAppleIIData data,
					int *output, int *input,
					int width, int height);

#endif
