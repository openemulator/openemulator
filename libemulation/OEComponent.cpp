
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

bool OEComponent::setData(const string &name, OEData *data)
{
	return false;
}

bool OEComponent::getData(const string &name, OEData **data)
{
	return false;
}

bool OEComponent::setResource(const string &name, OEData *data)
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
	OEObservers::iterator first = observerMap[notification].begin();
	OEObservers::iterator last = observerMap[notification].end();
	OEObservers::iterator i = remove(first, last, component);
	
	if (i != last)
		observerMap[notification].erase(i, last);
	
	return (i != last);
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

int OEComponent::ioctl(int command, void *data)
{
	return false;
}

OEUInt8 OEComponent::read(OEUInt32 address)
{
	return 0;
}

void OEComponent::write(OEUInt32 address, OEUInt8 value)
{
}

OEUInt16 OEComponent::readw(OEUInt32 address)
{
	return 0;
}

void OEComponent::writew(OEUInt32 address, OEUInt16 value)
{
}

OEUInt32 OEComponent::readd(OEUInt32 address)
{
	return 0;
}

void OEComponent::writed(OEUInt32 address, OEUInt32 value)
{
}

bool OEComponent::readBlock(OEUInt32 address, OEData *value)
{
	return false;
}

bool OEComponent::writeBlock(OEUInt32 address, const OEData *value)
{
	return false;
}

bool OEComponent::setMemoryMap(OEComponent *component, const string &value)
{
	return false;
}

bool OEComponent::getMemoryMap(string &value)
{
	return false;
}

int OEComponent::getInt(const string &value)
{
	if (value.substr(0, 2) == "0x")
	{
		unsigned int i;
		stringstream ss;
		ss << hex << value.substr(2);
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
	stringstream ss;
	ss << value;
	return ss.str();
}

string OEComponent::getHex(int value)
{
	stringstream ss;
	ss << "0x" << hex << value;
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
		stringstream ss;
		ss << hex << value.substr(start + i * 2, 2);
		ss >> n;
		result[i] = n;
	}
	
	return result;
}

int OEComponent::getNextPowerOf2(int value)
{
	return (int) pow(2, ceil(log2(value)));
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
			OELog("memory range '" + value + "' invalid");
			return false;
		}
		
		ranges.push_back(range);
		
		startPos = value.find_first_not_of(',', endPos);
		endPos = value.find_first_of(',', startPos);
	}
	
	return true;
}
