
/**
 * libemulator
 * Host Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host video events
 */

#include "OEComponent.h"

// Registers virtual screens (add, remove). Components provide the HostVideoFrame
// Provides these frames to the screen update function

// Messages
enum
{
	HOST_VIDEO_ADD_SCREEN = OEIOCTL_USER,
	HOST_VIDEO_REMOVE_SCREEN,
	HOST_VIDEO_GET_SCREEN_NUM,
	HOST_VIDEO_GET_SCREEN,
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

class HostVideo : public OEComponent
{

};
