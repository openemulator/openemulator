
/**
 * libntsc
 * NTSC emulation library
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements high-quality NTSC video emulation.
 */

/**
 * Notes on the processing:
 *
 * - Image widths should be multiple of 24.
 * - Pixels are represented using this 32-bit format:
 *   0ooRRRR RRRRooGG GGGGGGoo BBBBBBBB
 *   (R, G, B are the color components, o are overflow bits)
 * - Pixels can be added together with simple integer addition
 * - Packed color component range 0x0XX is clamped to 0x00
 * - Packed color component range 0x1XX is mapped to 0x00..0xff
 * - Packed color component range 0x2XX-0x3XX is clamped to 0xff
 *
 * Acknoledgements to blargg and newrisingsun for their
 * research, implementation and optimization of NTSC emulation.
 */

#ifndef _NTSC_H
#define _NTSC_H

#define NTSC_DECODERMATRIX_DIM	3
#define NTSC_DECODERMATRIX_SIZE	9

#ifdef __cplusplus
extern "c" {
#endif

typedef struct
{
	float brightness;
	float contrast;
	float saturation;
	float hue;
	
	float lumaBandwidth;
	float chromaBandwidth;
	
	const double *decoderMatrix;
} NTSCConfiguration;

extern const double ntscStandardRGBToYUV[NTSC_DECODERMATRIX_SIZE];
extern const double ntscCXA2025ASRGBToYUV[NTSC_DECODERMATRIX_SIZE];

#include "ntscAppleII.h"
#include "ntscCGA.h"

#ifdef __cplusplus
}
#endif

#endif
