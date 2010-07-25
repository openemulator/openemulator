
/**
 * libemulation
 * OEAddress
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Type for transforming DML addresses
 */

#ifndef _OEREF_H
#define _OEREF_H

#include "OETypes.h"

#define OE_DEVICE_SEP "::"
#define OE_COMPONENT_SEP "."

class OEAddress
{
public:
	OEAddress();
	OEAddress(string ref);
	
	void setDevice(string value);
	string getDevice() const;
	
	void setComponent(string value);
	string getComponent() const;
	
	void setProperty(string value);
	string getProperty() const;
	
	string ref();
	string ref(string value);
	string relativeRef(string value);
	
private:
	string device;
	string component;
	string property;
	
	string filter(string value);
};

#endif
