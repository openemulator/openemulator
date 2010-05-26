
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

void OEComponent::addObserver(OEObserverList &o, OEComponent *component)
{
	o.push_back(component);
}

void OEComponent::removeObserver(OEObserverList &o, OEComponent *component)
{
	OEObserverList::iterator iterator;
	for (iterator = o.begin();
		 iterator != o.end();
		 iterator++)
	{
		if (*iterator == component)
		{
			o.erase(iterator);
			break;
		}
	}
}

void OEComponent::postNotification(int id, OEObserverList &o, void *data)
{
	OENotification notification;
	notification.id = id;
	notification.component = this;
	notification.data = data;
	
	vector<OEComponent *>::iterator iterator;
	for (iterator = o.begin();
		 iterator != o.end();
		 iterator++)
		(*iterator)->ioctl(OE_NOTIFY, &notification);
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

int OEComponent::getPreviousPowerOf2(int value)
{
	return (int) pow(2, floor(log2(value)));
}
