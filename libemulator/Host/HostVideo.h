
/**
 * libemulator
 * Host Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host video events
 */

#include "OEComponent.h"

// Messages
enum
{
	HOST_VIDEO_ADD_SCREEN,
	HOST_VIDEO_REMOVE_SCREEN,
	HOST_VIDEO_GET_SCREEN,
	HOST_VIDEO_SET_SCREEN,
};

typedef struct
{
	int *framebufferData;
	int framebufferWidth;
	int framebufferHeight;
	int contentWidth;
	int contentHeight;
	int screenWidth;
	int screenHeight;
} HostVideoFrame;

// Calculation of contents
//
// xn = ceil(sqrt(N))
// un = ceil(N/xn)
//
// The size of each screen is:
// maxWidth * xn
// maxHeight * yn

class HostAudio : public OEComponent
{

};
