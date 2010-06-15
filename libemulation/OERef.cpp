
/**
 * libemulation
 * DML Reference
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Type for representing DML refs
 */

#include <iostream>

#include "OERef.h"

OERef::OERef()
{
}

OERef::OERef(string stringRef)
{
	int deviceIndex = stringRef.find(OE_DEVICE_SEP);
	if (deviceIndex != string::npos)
	{
		device = stringRef.substr(0, deviceIndex);
		deviceIndex += sizeof(OE_DEVICE_SEP) - 1;
		
		int componentIndex = stringRef.find(OE_COMPONENT_SEP, deviceIndex);
		if (componentIndex != string::npos)
		{
			component = stringRef.substr(deviceIndex, componentIndex - deviceIndex);
			componentIndex += sizeof(OE_COMPONENT_SEP) - 1;
			property = stringRef.substr(componentIndex);
		}
		else
			component = stringRef.substr(deviceIndex);
	}
	else
		device = stringRef;
}

OERef::OERef(const OERef &ref)
{
	device = ref.device;
	component = ref.component;
	property = ref.property;
}

OERef &OERef::operator=(const OERef &ref)
{
	device = ref.device;
	component = ref.component;
	property = ref.property;
	
	return *this;
}

bool OERef::operator==(const OERef &ref)
{
	if (device != ref.device)
		return false;
	
	if (component != ref.component)
		return false;
	
	if (property != ref.property)
		return false;
	
	return true;
}

ostream &operator<<(ostream &stream, const OERef &ref)
{
	if (ref.getDevice().size())
	{
		stream << ref.getDevice();
		
		if (ref.getComponent().size())
		{
			stream << string("::") << ref.getComponent();
			
			if (ref.getProperty().size())
				stream << string(".") << ref.getProperty();
		}
	}
	
	return stream;
}

bool OERef::isEmpty()
{
	return (!device.size());
}

void OERef::setDevice(string device)
{
	this->device = device;
}

string OERef::getDevice() const
{
	return device;
}

OERef OERef::getDeviceRef()
{
	return OERef(getDevice());
}

void OERef::setComponent(string component)
{
	this->component = component;
}

string OERef::getComponent() const
{
	return component;
}

OERef OERef::getComponentRef()
{
	return OERef(getDevice() + "::" + getComponent());
}

void OERef::setProperty(string property)
{
	this->property = property;
}

string OERef::getProperty() const
{
	return property;
}

OERef OERef::getRef(string stringRef)
{
	if (!stringRef.size())
		return *this;
	else if (stringRef.find("::") == string::npos)
		return OERef(getDevice() + "::" + stringRef);
	else
		return OERef(stringRef);
}

string OERef::getStringRef()
{
	if (!device.size())
		return "";
	else if (!component.size())
		return device;
	else if (!property.size())
		return device + "::" + component;
	else
		return device + "::" + component + "." + property;
}

string OERef::getStringRef(string stringRef)
{
	return getRef(stringRef).getStringRef();
}
