
/**
 * libntsc
 * NTSC emulation library
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements high-quality NTSC video emulation.
 */

#include "ntsc.h"

const float ntscStandardRGBToYUV[] = 
{
	1.0,		0.0,		1.139883,
	1.0,		-0.394642,	-0.580622,
	1.0,		2.032062,	0,
};

const float ntscCXA2025ASRGBToYUV[] = 
{
	1.0,		1.630,		0.317,
	1.0,		-0.378,		-0.466,
	1.0,		1.089,		1.677,
};
