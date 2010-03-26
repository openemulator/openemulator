
/**
 * libntsc
 * NTSC emulation library
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements high-quality NTSC video emulation.
 */

typedef struct
{
	float brightness;
	float contrast;
	float saturation;
	float hue;
	
	float lumaBandwidth;
	float chromaBandwidth;
} NTSCConfiguration;

#include "ntscAppleII.h"
