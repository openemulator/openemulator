
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

// getScreen returns a vector of frames to be updated

// positionLeft and positionTop is updated through:
// XC = ceil(sqrt(N))
// YC = ceil(N / XC)

// Messages
enum
{
	HOSTVIDEO_ADD_SCREEN,
	HOSTVIDEO_REMOVE_SCREEN,
	HOSTVIDEO_GET_FRAME,
	HOSTVIDEO_DRAW_FRAME,
	
	HOSTVIDEO_GET_SCREENS,
	HOSTVIDEO_SET_WINDOWSIZE,
	HOSTVIDEO_GET_WINDOWSIZE,
	HOSTVIDEO_GET_ACTUALWINDOWSIZE,
};

typedef struct
{
	double timeStamp;
	int *framebufferData;
	int framebufferWidth;
	int framebufferHeight;
	int contentWidth;
	int contentHeight;
	int screenWidth;
	int screenHeight;
	int screenLeft;
	int screenTop;
	bool isUpdated;
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
public:
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	
	int ioctl(int message, void *data);
	
private:
	vector<HostVideoFrame *> videoFrames;
};
