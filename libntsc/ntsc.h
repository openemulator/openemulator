
/**
 * libntsc
 * NTSC emulation library
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements high-quality NTSC video emulation.
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
	
	double *decoderMatrix;
	
	float lumaBandwidth;
	float chromaBandwidth;
} NTSCConfiguration;

#include "ntscAppleII.h"

	
#ifdef __cplusplus
}
#endif
		
#endif
