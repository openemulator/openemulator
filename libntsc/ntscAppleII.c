
/**
 * libntsc
 * Apple II NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple II NTSC emulation.
 */

#include "ntscAppleII.h"

int ntscAppleIIInit(NTSCAppleIIData *data, NTSCConfiguration *config)
{
	double w[NTSC_APPLEII_N];
	double wy[NTSC_APPLEII_N];
	double wc[NTSC_APPLEII_N];
	
	calculateLanczosWindow(wy, NTSC_APPLEII_N, config->lumaBandwidth);
	calculateChebyshevWindow(w, NTSC_APPLEII_N, NTSC_APPLEII_CHEBYSHEV_SIDELOBE_DB);
	applyWindow(wy, w, NTSC_APPLEII_N);

	calculateLanczosWindow(wc, NTSC_APPLEII_N, config->chromaBandwidth);
	calculateChebyshevWindow(w, NTSC_APPLEII_N, NTSC_APPLEII_CHEBYSHEV_SIDELOBE_DB);
	applyWindow(wc, w, NTSC_APPLEII_N);
}

int ntscAppleIIBlit(NTSCAppleIIData *data,
					int *output, int *input,
					int width, int height)
{
	
}
