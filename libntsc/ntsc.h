
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
 * Acknowledgemends to blargg and newrisingsun for their
 * research, implementation and optimization of NTSC emulation.
 */

#ifndef _NTSC_H
#define _NTSC_H

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
	
	const float *decoderMatrix;
} NTSCConfiguration;

extern const float ntscStandardRGBToYUV[];
extern const float ntscCXA2025ASRGBToYUV[];

#include "ntsc1Bit.h"
#include "ntsc2Bit.h"

#ifdef __cplusplus
}
#endif

#endif
