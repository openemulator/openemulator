
/**
 * libemulation
 * DML Reference
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Type for representing DML refs
 */

#ifndef _OEREF_H
#define _OEREF_H

#include "OETypes.h"

#define OE_DEVICE_SEP "::"
#define OE_COMPONENT_SEP "."

class OERef
{
public:
	OERef();
	OERef(string stringRef);
	OERef(const OERef &ref);
	OERef &operator=(const OERef &ref);
	bool operator==(const OERef &ref);
	
	bool isEmpty();
	
	void setDevice(string device);
	string getDevice() const;
	OERef getDeviceRef();
	
	void setComponent(string component);
	string getComponent() const;
	
	void setProperty(string property);
	string getProperty() const;
	OERef getComponentRef();
	
	OERef getRef(string stringRef);
	
	string getStringRef();
	string getStringRef(string stringRef);
	
private:
	string device;
	string component;
	string property;
};

#endif
