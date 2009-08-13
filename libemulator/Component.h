
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

#define IOCTL_OK	0
#define IOCTL_ERROR	1

using namespace std;

class Component {
public:
	Component();
	virtual ~Component();
	
	virtual void init();		// Missing: dictionary ref is sent here
	virtual void remove();		// Removes the component
	virtual void terminate();	// Missing: dictionary ref is sent here
	
	virtual int ioctl(int message, void * data);
	
	void addObserver(class Component * component);
	void removeObserver(class Component * component);
	virtual void onNotification(class Component * component);
	
	virtual int read(int address);
	virtual void write(int address, int value);
	
protected:
	void postNotification();
	
private:
	vector<class Component *> observers;
};

#endif
