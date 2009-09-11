
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


enum {
	IOCTL_OPEN,
	IOCTL_SAVE,
	IOCTL_CONNECT,
	IOCTL_SETPROPERTY,
	IOCTL_GETPROPERTY,
	IOCTL_FRAMESTART,
	IOCTL_FRAMEEND,
	IOCTL_UIUPDATE,
	IOCTL_CLIPBOARDCOPY,
	IOCTL_CLIPBOARDPASTE,
	IOCTL_AUDIOPLAY,
	IOCTL_AUDIOPLAYSTOP,
	IOCTL_AUDIORECORD,
	IOCTL_AUDIORECORDSTOP,
	IOCTL_MOUNT,
	IOCTL_UNMOUNT,
};

struct DMLPaths
{
	const string &emulationPath;
	const string &resourcesPath;
};

struct DMLProperty
{
	string key;
	string value;
};

struct DMLConnection
{
	string key;
	Component *component;
};

#endif
