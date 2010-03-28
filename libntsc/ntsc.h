
/**
 * libntsc
 * NTSC emulation library
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements high-quality NTSC video emulation.
 */

/*
 Notes on the internal pixel format:

 * Packed pixel format is 0xxRRRR RRRRxxGG GGGGGGxx BBBBBBBB
   (xx are junk bits)
 * Packed color components are added a 0x100 offset
 * Packed pixels can be added together and provide RGB mixing 
 * Color component range 0x0XX is clamped to 0
 * Color component range 0x1XX is linear
 * Color component range 0x2XX and 0x3XX is clamped to 0xff
 
 */

#ifndef _NTSC_H
#define _NTSC_H

#define NTSC_COLORGAIN		255.0
#define NTSC_COLOROFFSET	256.5

#define NTSC_CLAMP_ANDMASK	(1 << 28) | (1 << 18) | (1 << 8)
#define NTSC_CLAMP_ORMASK	(1 << 29) | (1 << 19) | (1 << 9)

// Macros
#define NTSC_PACK(r, g, b)\
	(((int) r << 20) | ((int) g << 10) | ((int) b))

#define NTSC_UNPACK(value)\
	(value >> 4 & 0xff0000) | (value >> 2 & 0x00ff00) | (value & 0x0000ff)

#define NTSC_CLAMP(value)\
{\
	int andMask = (value & NTSC_CLAMP_ANDMASK);\
	int orMask = (value & NTSC_CLAMP_ORMASK);\
	andMask -= (andMask >> 8);\
	orMask -= (orMask >> 9);\
	value &= andMask;\
	value |= orMask;\
}

#ifdef __cplusplus
extern "c" {
#endif

typedef struct
{
	float brightness;
	float contrast;
	float saturation;
	float hue;
	
	double *decoderMatrix;
	
	float lumaBandwidth;
	float chromaBandwidth;
} NTSCConfiguration;

#include "ntscAppleII.h"
#include "ntscCGA.h"

#ifdef __cplusplus
}
#endif
		
#endif
