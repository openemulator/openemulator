
/**
 * libemulation
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

bool OEComponent::setMemoryMap(OEComponent *component, const string &value)
{
	return false;
}

bool OEComponent::getMemoryMap(string &value)
{
	return false;
}

int OEComponent::ioctl(int command, void *data)
{
	return 0;
}

OEUInt8 OEComponent::read(int address)
{
	return 0;
}

void OEComponent::write(int address, OEUInt8 value)
{
}

OEUInt16 OEComponent::read16(int address)
{
	return 0;
}

void OEComponent::write16(int address, OEUInt16 value)
{
}

OEUInt32 OEComponent::read32(int address)
{
	return 0;
}

void OEComponent::write32(int address, OEUInt32 value)
{
}

bool OEComponent::read(int address, OEData &value)
{
	return false;
}

bool OEComponent::write(int address, OEData &value)
{
	return false;
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

bool OEComponent::getRange(OEMemoryRange &range, const string &value)
{
	range.read = false;
	range.write = false;
	
	size_t pos = 0;
	
	while(1)
	{
		if (pos == value.size())
			return false;
		else if ((value[pos] == 'R') || (value[pos] == 'r'))
			range.read = true;
		else if ((value[pos] == 'W') || (value[pos] == 'w'))
			range.write = true;
		else
			break;
	}
	
	if (!range.read && !range.write)
		range.read = range.write = true;
	
	size_t separatorPos = value.find_first_of('-', pos);
	if (separatorPos == string::npos)
		range.end = range.start = getInt(value.substr(pos));
	else
	{
		range.start = getInt(value.substr(pos, separatorPos));
		range.end = getInt(value.substr(separatorPos + 1));
	}
	
	if (range.start > range.end)
		return false;
	
	return true;
}

bool OEComponent::getRanges(OEMemoryRanges &ranges, const string &value)
{
	size_t startPos = value.find_first_not_of(',', 0);
	size_t endPos = value.find_first_of(',', startPos);
	
	while ((startPos != string::npos) || (endPos != string::npos))
	{
		OEMemoryRange range;
		
		if (!getRange(range, value.substr(startPos, endPos - startPos)))
		{
			OELog("memory range \"" + value + "\" invalid");
			return false;
		}
		
		ranges.push_back(range);
		
		startPos = value.find_first_not_of(',', endPos);
		endPos = value.find_first_of(',', startPos);
	}
	
	return true;
}
