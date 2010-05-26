
/**
 * libntsc
 * NTSC emulation library
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements high-quality NTSC video emulation.
 */

/*

==Configuration settings==

* lumaBandwidth: bandwidth of the low-pass luminance filter.
  Range: 0.0 (0 MHz) to 1.0 (2 * 3.57 MHz)
* brightness: black-level offset. Range: -1.0 to 1.0
* contrast: black-white dynamic range. Range: 0.0 to 2.0

* chromaBandwidth: bandwidth of the band-pass chrominance filter
  Range: 0.0 (0 MHz) to 0.5 (3.57 MHz)
* colorize: if set to 1, sets UV coordinates to (Y / sqrt(2), 0).
* saturation: scaling of UV coordinates. Range: 0.0 to 1.0
* hue: rotation of UV coordinates. Range: 0.0 (0 deg.) to 1.0 (360 deg.)

* decoderMatrix: the YUV to RGB coordinate change

==Internal signal format==

 * Image width must be a multiple of 24.
* Pixel color is represented with a 32-bit int:
  
  0ooRRRR RRRRooGG GGGGGGoo BBBBBBBB
  
  (R, G, B are the color components, o are overflow bits)
  
* Pixels can be added together with a simple integer addition
* Packed color component range 0x0XX is clamped to 0x00
* Packed color component range 0x1XX is mapped to 0x00..0xff
* Packed color component range 0x2XX-0x3XX is clamped to 0xff

==Acknowledgements==

Acknowledgements to blargg and newrisingsun for their
research, implementation and optimization of NTSC emulation.

*/

#ifndef _NTSC_H
#define _NTSC_H

#ifdef __cplusplus
extern "c" {
#endif

typedef struct
{
	// Y - Luma
	float lumaBandwidth;
	float brightness;
	float contrast;
	
	// C - Chroma
	float chromaBandwidth;
	int colorize;
	float saturation;
	float hue;
	
	// YUV to RGB
	const float *decoderMatrix;
} NTSCConfiguration;

extern const float ntscStandardRGBToYUV[];
extern const float ntscCXA2025ASRGBToYUV[];

#include <math.h>

#include "ntsc1Bit.h"
#include "ntscCGA.h"

#ifdef __cplusplus
}
#endif

#endif
