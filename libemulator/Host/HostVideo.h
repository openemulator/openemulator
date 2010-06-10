
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
	
	HOSTVIDEO_ADD_SCREEN,
	HOSTVIDEO_REMOVE_SCREEN,
	HOSTVIDEO_UPDATE_SCREEN,
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

typedef void (*HostVideoObserver)(HostVideoFrames &frames);

class HostVideo : public OEComponent
{
public:
	HostVideo();
	
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	
	int ioctl(int message, void *data);
	
private:
	string window;
	HostVideoFrames frames;
	
	HostVideoObserver observer;
};
