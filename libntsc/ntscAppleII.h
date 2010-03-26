
/**
 * libntsc
 * Apple II NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple II NTSC emulation.
 */

typedef struct NTSCAppleIIData;

int ntscAppleIIInit(NTSCAppleIIData *data, NTSCConfiguration *config);
int ntscAppleIIBlit(NTSCAppleIIData *data,
					int *output, int *input,
					int width, int height);
