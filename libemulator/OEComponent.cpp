
/**
 * libemulator
 * OEComponent
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Component type
 */

#include "math.h"
#include <sstream>

#include "OEComponent.h"

OEComponent::OEComponent()
{
}

OEComponent::~OEComponent()
{
}

int OEComponent::ioctl(int command, void *data)
{
	return 0;
}

int OEComponent::read(int address)
{
	return 0;
}

void OEComponent::write(int address, int value)
{
}

void OEComponent::addObserver(OEComponent *component)
{
	observers.push_back(component);
}

void OEComponent::removeObserver(OEComponent *component)
{
	vector<OEComponent *>::iterator iterator;
	for (iterator = observers.begin();
		 iterator != observers.end();
		 iterator++)
	{
		if (*iterator == component)
			observers.erase(iterator);
	}
}

void OEComponent::onNotification(OEComponent *component, int message, void *data)
{
}

void OEComponent::postNotification(int message, void *data)
{
	vector<OEComponent *>::iterator iterator;
	for (iterator = observers.begin();
		 iterator != observers.end();
		 iterator++)
		(*iterator)->onNotification(this, message, data);
}

int intValue(string value)
{
	if (value.substr(0, 2) == "0x")
	{
		unsigned int i;   
		std::stringstream ss;
		ss << std::hex << value.substr(2);
		ss >> i;
	}
	return atoi(value.c_str());
}

int nextPowerOf2(int value)
{
	return (int) pow(2, ceil(log2(value)));
}
