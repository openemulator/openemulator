
/**
 * libntsc
 * Apple II NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple II NTSC emulation.
 */

#include "ntsc.h"
#include "ntscSignalProcessing.h"

int ntscAppleIIInit(NTSCAppleIIData *data, NTSCConfiguration *config)
{
	double w[NTSC_APPLEII_N];
	double wy[NTSC_APPLEII_N];
	double wc[NTSC_APPLEII_N];
	double decoderMatrix[NTSC_DECODERMATRIX_SIZE];
	
	if (!calculateChebyshevWindow(w, NTSC_APPLEII_N,
								  NTSC_APPLEII_CHEBYSHEV_SIDELOBE_DB))
		return 0;
	
	calculateLanczosWindow(wy, NTSC_APPLEII_N, config->lumaBandwidth);
	multiplyWindow(wy, w, NTSC_APPLEII_N);
	
	calculateLanczosWindow(wc, NTSC_APPLEII_N, config->chromaBandwidth);
	multiplyWindow(wc, w, NTSC_APPLEII_N);
	
	copyDecoderMatrix(decoderMatrix, config->decoderMatrix);
	transformDecoderMatrix(decoderMatrix, config->saturation, config->hue);
	
	return 1;
}

int ntscAppleIIBlit(NTSCAppleIIData *data,
					int *output, int *input,
					int width, int height)
{
	return 1;
}
