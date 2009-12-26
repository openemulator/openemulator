/*
 *  OERef.cpp
 *  OpenEmulator
 *
 *  Created by Marc S. Reßl on 23/12/09.
 *  Copyright 2009 ITBA. All rights reserved.
 *
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

bool OERef::isValid()
{
	return (device.size() != 0);
}

string OERef::getDevice() const
{
	return device;
}

string OERef::getComponent() const
{
	return component;
}

string OERef::getProperty() const
{
	return property;
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

OERef OERef::buildRef(string relativeStringRef)
{
	if (relativeStringRef.size() == 0)
		return OERef(getStringRef());
	else if (relativeStringRef.find("::") == string::npos)
		return OERef(getDevice() + "::" + relativeStringRef);
	else
		return OERef(relativeStringRef);
}
