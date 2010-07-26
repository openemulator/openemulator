
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

OEAddress::OEAddress(string ref)
{
	int index;
	
	index = ref.find(OE_DEVICE_SEP);
	if (index != string::npos)
	{
		setDevice(ref.substr(0, index));
		ref = ref.substr(index + sizeof(OE_DEVICE_SEP) - 1);
	}
	
	index = ref.find(OE_COMPONENT_SEP);
	if (index != string::npos)
	{
		setProperty(ref.substr(index + sizeof(OE_COMPONENT_SEP) - 1));
		ref = ref.substr(0, index);
	}
	
	setComponent(ref);
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

string OEAddress::ref()
{
	string ref;
	
	if (device != "")
		ref += device + "::";
	
	ref += component;
	
	if (property != "")
		ref += "." + property;
	
	return ref;
}

string OEAddress::ref(string value)
{
	OEAddress address = OEAddress(value);
	
	if (address.getDevice() == "")
		address.setDevice(device);
	if (address.getComponent() == "")
		address.setComponent(component);
	if (address.getProperty() == "")
		address.setProperty(property);
	
	return address.ref();
}

string OEAddress::relativeRef(string value)
{
	OEAddress address = OEAddress(value);
	
	if (address.getDevice() == device)
		address.setDevice("");
	if (address.getComponent() == component)
		address.setComponent("");
	if (address.getProperty() == property)
		address.setProperty("");
	
	return address.ref();
}

string OEAddress::filter(string value)
{
	string out;
	
	for (string::iterator i = value.begin();
		 i != value.end();
		 i++)
	{
		if (((*i) != ':') || ((*i) != '.'))
			out += *i;
	}
	
	return out;
}
