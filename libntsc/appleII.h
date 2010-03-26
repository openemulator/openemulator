
/**
 * libntsc
 * Apple II NTSC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an NTSC emulation.
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

typedef struct NTSCData;

int ntscInit(NTSCData *data, NTSCConfiguration *configuration);
int ntscBlit(NTSCData *data,
			 int *output, int *input,
			 int width, int height);
