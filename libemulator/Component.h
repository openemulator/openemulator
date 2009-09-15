
/**
 * libemulator
 * Component interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

class Component
{
public:
	Component();
	virtual ~Component();
	
	virtual int ioctl(int message, void *data);
	virtual int read(int address);
	virtual void write(int address, int value);
	
	void addObserver(class Component *component);
	void removeObserver(class Component *component);
	virtual void onNotification(class Component *component);
	
protected:
	void postNotification();
	
private:
	vector<class Component *> observers;
};


enum {
	IOCTL_INIT = 0,
	IOCTL_SETPROPERTY,
	IOCTL_GETPROPERTY,
	IOCTL_CLIPBOARDCOPY,
	IOCTL_CLIPBOARDPASTE,
	IOCTL_MOUNT,
	IOCTL_UNMOUNT,
	IOCTL_UIUPDATE,
};

struct DMLConnection
{
	string key;
	Component *component;
};

struct DMLProperty
{
	string key;
	string value;
};

struct DMLData
{
	string key;
	int dataSize;
	char *data;
};

#endif
