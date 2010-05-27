
/**
 * libemulator
 * OEComponent
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Component definition
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

bool OEComponent::setProperty(string name, string value)
{
	return false;
}

bool OEComponent::getProperty(string name, string &value)
{
	return false;
}

bool OEComponent::setData(string name, OEData &data)
{
	return false;
}

bool OEComponent::getData(string name, OEData &data)
{
	return false;
}

bool OEComponent::setResource(string name, OEData &data)
{
	return false;
}

bool OEComponent::connect(string name, OEComponent *component)
{
	return false;
}

bool OEComponent::addObserver(OEComponent *component,
							  int notification)
{
	observers[notification].push_back(component);
	
	return true;
}

bool OEComponent::removeObserver(OEComponent *component,
								 int notification)
{
	
	OEObservers::iterator i;
	for (i = observers[notification].begin();
		 i != observers[notification].end();
		 i++)
	{
		if (*i == component)
		{
			observers[notification].erase(i);
			return true;
		}
	}
	
	return false;
}

void OEComponent::postNotification(int notification, void *data)
{
	OEObservers::iterator i;
	for (i = observers[notification].begin();
		 i != observers[notification].end();
		 i++)
		(*i)->notify(notification, this, data);
}

void OEComponent::notify(int notification,
						 OEComponent *component,
						 void *data)
{
}

bool OEComponent::setMemoryMap(OEComponent *component,
							   OEMemoryRange &range)
{
	return false;
}

bool OEComponent::getMemoryMap(OEMemoryRange &range)
{
	return false;
}

bool OEComponent::assertInterrupt(int id)
{
	return false;
}

bool OEComponent::releaseInterrupt(int id)
{
	return false;
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

int OEComponent::getInt(string value)
{
	if (value.substr(0, 2) == "0x")
	{
		unsigned int i;
		std::stringstream ss;
		ss << std::hex << value.substr(2);
		ss >> i;
		return i;
	}
	else
		return atoi(value.c_str());
}

double OEComponent::getFloat(string value)
{
	return atof(value.c_str());
}

string OEComponent::getString(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

string OEComponent::getHex(int value)
{
	std::stringstream ss;
	ss << "0x" << std::hex << value;
	return ss.str();
}

vector<char> OEComponent::getCharVector(string value)
{
	vector<char> result;
	
	if (value.substr(0, 2) == "0x")
		value = value.substr(2);
	
	int size = value.size() / 2;
	result.resize(size);
	
	for (int i = 0; i < size; i++)
	{
		unsigned int n;
		std::stringstream ss;
		ss << std::hex << value.substr(i * 2, 2);
		ss >> n;
		result[i] = n;
	}
	
	return result;
}

int OEComponent::getLowerPowerOf2(int value)
{
	return (int) pow(2, floor(log2(value)));
}

string OEComponent::getRange(int start, int end)
{
	
}

