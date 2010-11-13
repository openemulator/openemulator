
/**
 * libemulation
 * Video interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host video interface
 */

typedef struct
{
	int *frameData;
	int frameWidth;
	int frameHeight;
} HostCameraFrame;

class OEVideo
{
	HostCameraFrame *getFrame();
};
