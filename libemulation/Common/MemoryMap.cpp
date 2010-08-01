
/**
 * libemulation
 * Memory map
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic memory map
 */

#include <iostream>

#include "MemoryMap.h"

bool MemoryMap::setProperty(const string &name, const string &value)
{
	if (name == "mask")
		mask = getInt(value);
	else if (name == "shift")
		shift = getInt(value);
	else if (name == "mmuMap")
		mmuMap = value;
	else
		return false;
	
	return true;
}

bool MemoryMap::connect(const string &name, OEComponent *component)
{
	if (name == "floatingBus")
		floatingBus = component;
	else
		return false;
	
	return true;
}

bool MemoryMap::postEvent(OEComponent *component, int notification, void *data)
{
	string value = *((string *) data);
	OEMemoryRanges ranges;
	
	if (!getRanges(ranges, value))
		return false;
	
	for (OEMemoryRanges::iterator i = ranges.begin();
		 i != ranges.end();
		 i++)
	{
		if ((i->end >= mask) ||
			(i->start & 0xff) || ((i->end & 0xff) != 0xff))
		{
			OELog("memory range " + value + "invalid");
			return false;
		}
		
		int startPage = i->start >> 8;
		int endPage = i->end >> 8;
		
		for (int j = startPage; j < endPage; j++)
		{
			if (i->read)
				readMap[j] = component;
			if (i->write)
				writeMap[j] = component;
		}
	}
	
	return true;
}

OEUInt8 MemoryMap::read(int address)
{
	return readMap[(address & mask) >> shift]->read(address);
}

void MemoryMap::write(int address, OEUInt8 value)
{
	writeMap[(address & mask) >> shift]->write(address, value);
}

bool MemoryMap::getRange(OEMemoryRange &range, const string &value)
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

bool MemoryMap::getRanges(OEMemoryRanges &ranges, const string &value)
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
