
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

bool OEComponent::setProperty(const string &name, const string &value)
{
	return false;
}

bool OEComponent::getProperty(const string &name, string &value)
{
	return false;
}

bool OEComponent::setData(const string &name, const OEData &data)
{
	return false;
}

bool OEComponent::getData(const string &name, OEData &data)
{
	return false;
}

bool OEComponent::setResource(const string &name, const OEData &data)
{
	return false;
}

bool OEComponent::connect(const string &name, OEComponent *component)
{
	return false;
}

bool OEComponent::addObserver(OEComponent *component,
							  int notification)
{
	observerMap[notification].push_back(component);
	
	return true;
}

bool OEComponent::removeObserver(OEComponent *component,
								 int notification)
{
	
	OEObservers::iterator i;
	for (i = observerMap[notification].begin();
		 i != observerMap[notification].end();
		 i++)
	{
		if (*i == component)
		{
			observerMap[notification].erase(i);
			return true;
		}
	}
	
	return false;
}

void OEComponent::postNotification(int notification, void *data)
{
	OEObservers::iterator i;
	for (i = observerMap[notification].begin();
		 i != observerMap[notification].end();
		 i++)
		(*i)->notify(notification, this, data);
}

void OEComponent::notify(int notification,
						 OEComponent *component,
						 void *data)
{
}

bool OEComponent::setMemoryMap(OEComponent *component, const string &range)
{
	return false;
}

bool OEComponent::getMemoryMap(string &range)
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

int OEComponent::getInt(const string &value)
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

double OEComponent::getFloat(const string &value)
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

OEData OEComponent::getCharVector(const string &value)
{
	OEData result;
	int start = (value.substr(0, 2) == "0x") ? 2 : 0;
	int size = value.size() / 2 - start;
	result.resize(size);
	
	for (int i = 0; i < size; i++)
	{
		unsigned int n;
		std::stringstream ss;
		ss << std::hex << value.substr(start + i * 2, 2);
		ss >> n;
		result[i] = n;
	}
	
	return result;
}

int OEComponent::getLowerPowerOf2(int value)
{
	return (int) pow(2, floor(log2(value)));
}

OEMemoryRanges OEComponent::getRanges(const string &ranges)
{
	OEMemoryRanges result;
	
	size_t startPos = ranges.find_first_not_of(',', 0);
	size_t endPos = ranges.find_first_of(',', startPos);
	
	while ((startPos != string::npos) || (endPos != string::npos))
	{
		string range = ranges.substr(startPos, endPos - startPos);
		
		bool isRead = false;
		bool isWrite = false;
		
		while (range[0] == 'R' do {
			if (range[0] == 'R')
			{
				isRead = true;
			}
		else if (range[0] == 'W')
			isWrite = true;
		if (!isRead && !isWrite)
			isRead = isWrite = true;
		
		size_t separatorPos = range.find_first_of('-');
		if (separatorPos == string::npos)
		{
			OELog("range \"" + ranges + "\" invalid");
			return false;
		}
		
		int start = getInt(range.substr(0, separatorPos));
		int end = getInt(range.substr(separatorPos + 1));
		if (start > end)
		{
			OELog("range \"" + ranges + "\" invalid");
			return false;
		}
		
		startPos = range.find_first_not_of(',', endPos);
		endPos = range.find_first_of(',', startPos);
	}
}
