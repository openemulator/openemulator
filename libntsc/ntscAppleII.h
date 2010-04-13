
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

// Parameters:
#define NTSC_APPLEII_FILTER_N	17

#define NTSC_APPLEII_CHEBYSHEV_SIDELOBE_DB	50.0

// Modifications require code refactoring:
#define NTSC_APPLEII_INPUTBITNUM	8
#define NTSC_APPLEII_INPUTSIZE		(1 << NTSC_APPLEII_INPUTBITNUM)
#define NTSC_APPLEII_INPUTSAMPLENUM	8
#define NTSC_APPLEII_OUTPUTSIZE		24

#define NTSC_APPLEII_INPUTBLACK	0

typedef int NTSCAppleII[NTSC_APPLEII_INPUTSIZE][NTSC_APPLEII_OUTPUTSIZE];

void ntscAppleIIInit(NTSCAppleII convolutionTable,
					 NTSCConfiguration *config);
void ntscAppleIIBlit(NTSCAppleII convolutionTable,
					 unsigned char *input, int width, int height,
					 int *output, int outputPitch, int doubleScanlines);

#endif
