
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

// Modifiable parameters:
#define NTSC_APPLEII_FILTER_N	17
#define NTSC_APPLEII_CHEBYSHEV_SIDELOBE_DB	50.0

// Do not modify these defines:
#define NTSC_APPLEII_SIGNALNUM	8
#define NTSC_APPLEII_SIGNALSIZE	(1 << NTSC_APPLEII_SIGNALNUM)
#define NTSC_APPLEII_BLOCKSIZE	24
#define NTSC_APPLEII_CHUNKSIZE	8
#define NTSC_APPLEII_OFFSETGAIN	(1.0 * NTSC_APPLEII_CHUNKSIZE / NTSC_APPLEII_BLOCKSIZE)

#define NTSC_APPLEII_SIGNALBLACK 0

typedef int NTSCAppleIISignalToPixel[NTSC_APPLEII_SIGNALSIZE][NTSC_APPLEII_BLOCKSIZE];

void ntscAppleIIInit(NTSCAppleIISignalToPixel signalToPixel,
					 NTSCConfiguration *config);
void ntscAppleIIBlit(NTSCAppleIISignalToPixel signalToPixel,
					 int *output, unsigned char *input,
					 int width, int height);

#endif
