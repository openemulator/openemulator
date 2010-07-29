
/**
 * libemulation
 * OEAddress
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Type for transforming DML addresses
 */

#include "OEAddress.h"

OEAddress::OEAddress()
{
}

OEAddress::OEAddress(string address)
{
	int index;
	
	index = address.find(OE_DEVICE_SEPARATOR);
	if (index != string::npos)
	{
		setDevice(address.substr(0, index));
		address = address.substr(index + sizeof(OE_DEVICE_SEPARATOR) - 1);
	}
	
	index = address.find(OE_CONNECTION_SEPARATOR);
	if (index != string::npos)
	{
		setProperty(address.substr(index + sizeof(OE_CONNECTION_SEPARATOR) - 1));
		address = address.substr(0, index);
	}
	
	setComponent(address);
}

void OEAddress::setDevice(string value)
{
	device = filter(value);
}

string OEAddress::getDevice() const
{
	return device;
}

void OEAddress::setComponent(string value)
{
	component = filter(value);
}

string OEAddress::getComponent() const
{
	return component;
}

void OEAddress::setProperty(string value)
{
	property = filter(value);
}

string OEAddress::getProperty() const
{
	return property;
}

string OEAddress::address()
{
	string address;
	
	address += device;
	address += "::";
	address += component;
	if (property != "")
		address += "." + property;
	
	return address;
}

string OEAddress::address(string ref)
{
	OEAddress address = OEAddress(ref);
	
	if (address.getDevice() == "")
		address.setDevice(device);
	
	return address.ref();
}

string OEAddress::ref(string address)
{
	OEAddress address = OEAddress(address);
	
	if (address.getDevice() == device)
		address.setDevice("");
	
	return address.ref();
}

string OEAddress::filter(string value)
{
	string out;
	
	for (string::iterator i = value.begin();
		 i != value.end();
		 i++)
	{
		if (isalpha(*i) || isnumber(*i))
			out += *i;
	}
	
	return out;
}
