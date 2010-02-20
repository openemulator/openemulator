
/**
 * libemulator
 * OEComponent
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Component type
 */

#ifndef _OECOMPONENT_H
#define _OECOMPONENT_H

#include <string>
#include <vector>

#define OE_IOCTL_USER_MESSAGES 8

// Messages
enum {
	COMPONENT_SET_CONNECTION = 0,
	COMPONENT_SET_PROPERTY,
	COMPONENT_GET_PROPERTY,
	COMPONENT_SET_DATA,
	COMPONENT_GET_DATA,
	COMPONENT_SET_RESOURCE,
};

using namespace std;

class OEComponent
{
public:
	OEComponent();
	virtual ~OEComponent();
	
	virtual int ioctl(int message, void *data);
	virtual int read(int address);
	virtual void write(int address, int value);
	
	void addObserver(class OEComponent *component);
	void removeObserver(class OEComponent *component);
	virtual void onNotification(class OEComponent *component, int message, void *data);
	
protected:
	void postNotification(int message, void *data);
	
private:
	vector<OEComponent *> observers;
};

typedef struct
{
	string name;
	OEComponent *component;
} OEIoctlConnection;

typedef struct
{
	string name;
	string value;
} OEIoctlProperty;

typedef struct
{
	string name;
	vector<char> data;
} OEIoctlData;

// ioctl's for:
// * next frame
// * update outlets
// * send power, reset, pause messages
// * send config messages
// * do copy, paste, isCopyAvailable
// * get/set video options
// * get/set audio volume
// * play/record audio
// * lock/unlock disk drives
// * mount/unmount disk drives
// * get/set component options
// * send keyboard messages
// * set mouse position and buttons
// * set joystick position and buttons
// * set graphics tablet position and buttons
// * get video frames

// Note: move pause and power off images to libemulator

#endif
