
/**
 * libemulator
 * Component interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <stdint.h>

enum {
	IOCTL_SETPROPERTY,
	IOCTL_GETPROPERTY,
	IOCTL_FRAMESTART,
	IOCTL_FRAMEEND,
	IOCTL_COMPONENTADDED,
	IOCTL_COMPONENTREMOVED,
	IOCTL_UIUPDATE,
	IOCTL_CLIPBOARDCOPY,
	IOCTL_CLIPBOARDPASTE,
	IOCTL_AUDIOPLAY,
	IOCTL_AUDIOPLAYSTOP,
	IOCTL_AUDIORECORD,
	IOCTL_AUDIORECORDSTOP,
	IOCTL_MOUNTDISKIMAGE,
	IOCTL_UNMOUNTDISKIMAGE,
};

using namespace std;

class Component
{
public:
	Component();
	virtual ~Component();
	
	virtual int ioctl(int message, void * data);
	virtual int read(int address);
	virtual void write(int address, int value);
	
	void addObserver(class Component * component);
	void removeObserver(class Component * component);
	virtual void onNotification(class Component * component);
	
protected:
	void postNotification();
	
private:
	vector<class Component *> observers;
};

#endif
