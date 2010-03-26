
/**
 * libntsc
 * NTSC emulation library
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements high-quality NTSC video emulation.
 */

enum
{
	NTSC_APPLEII,
	NTSC_CGA,
} NTSCModes;

typedef struct
{
	int mode;
	
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
