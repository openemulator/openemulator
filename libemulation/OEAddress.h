
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

#define OE_DEVICE_SEPARATOR ":"
#define OE_CONNECTION_SEPARATOR "."

class OEAddress
{
public:
	OEAddress();
	OEAddress(string address);
	
	string getDevice() const;
	string getComponent() const;
	string getProperty() const;
	
	string address();
	string address(string ref);
	string ref(string address);
	
private:
	string device;
	string component;
	string property;
	
	string filter(string value);
	
	void setDevice(string value);
	void setComponent(string value);
	void setProperty(string value);
};

#endif
