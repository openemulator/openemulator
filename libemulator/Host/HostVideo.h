
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
	HOSTVIDEO_REGISTER_HOST,
	HOSTVIDEO_UPDATE_RENDER,
	
	HOSTVIDEO_ADD_SCREEN,
	HOSTVIDEO_REMOVE_SCREEN,
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
	bool updated;
} HostVideoFrame;

typedef vector<HostVideoFrame *> HostVideoFrames;

typedef void (*HostVideoPostFrame)();
typedef bool (*HostVideoOpenDevice)(OEComponent *component);
typedef void (*HostVideoCloseDevice)();

typedef struct
{
	HostVideoPostFrame postFrame;
	HostVideoOpenDevice openDevice;
	HostVideoCloseDevice closeDevice;
} HostVideoCallbacks;

class HostVideo : public OEComponent
{
public:
	HostVideo::HostVideo();
	
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	
	int ioctl(int message, void *data);
	
private:
	string window;
	HostVideoFrames frames;
	
	HostVideoCallbacks callbacks;
};
