
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

#define NTSC_APPLEII_N	17
#define NTSC_APPLEII_CHEBYSHEV_SIDELOBE_DB	50.0

typedef struct {} NTSCAppleIIData;

int ntscAppleIIInit(NTSCAppleIIData *data, NTSCConfiguration *config);
int ntscAppleIIBlit(NTSCAppleIIData *data,
					int *output, int *input,
					int width, int height);

#endif
