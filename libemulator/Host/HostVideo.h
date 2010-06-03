
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
};

// Notifications
enum
{
	HOSTVIDEO_FRAME_DID_RENDER,
	HOSTVIDEO_FRAME_DID_CAPTURE,
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

class HostVideo : public OEComponent
{
public:
	HostVideo();
	
	bool setProperty(const string &name, const string &value);
	bool getProperty(const string &name, string &value);
	
	bool addObserver(OEComponent *component, int notification);
	bool removeObserver(OEComponent *component, int notification);
	void postNotification(int notification, void *data);
	
	int ioctl(int message, void *data);
	
private:
	string window;
	HostVideoFrames videoFrames;
	
	OEHostObserver hostObserver;
};
