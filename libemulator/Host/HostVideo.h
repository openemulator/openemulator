
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
	HOSTVIDEO_ADD_SCREEN,
	HOSTVIDEO_REMOVE_SCREEN,
	HOSTVIDEO_QUERY_FRAME,
	HOSTVIDEO_RETURN_FRAME,
	
	HOSTVIDEO_SET_CALLBACK,
};

// Structures
typedef struct
{
	int *framebufferData;
	int framebufferWidth;
	int framebufferHeight;
	int contentWidth;
	int contentHeight;
	int screenWidth;
	int screenHeight;
	int screenLeft;
	int screenTop;
	bool updated;
} HostVideoFrame;

typedef vector<HostVideoFrame> HostVideoFrames;

typedef void (*HostVideoCallback)(HostVideoFrames videoFrames, void *userData);

class HostVideo : public OEComponent
{
public:
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	
	int ioctl(int message, void *data);
	
private:
	string window;
	HostVideoFrames videoFrames;
};
