
/**
 * libemulation
 * Address decoder
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address decoder
 */

#include <iostream>

#include "AddressDecoder.h"

AddressDecoder::AddressDecoder()
{
	bus = NULL;
	
	mask = 0;
	shift = 0;

	readMap.resize(1);
	writeMap.resize(1);
}

void AddressDecoder::mapBus(OEComponent *bus)
{
	for (OEComponents::iterator i = readMap.begin();
		 i != readMap.end();
		 i++)
		if (!*i)
			*i = bus;
	
	for (OEComponents::iterator i = writeMap.begin();
		 i != writeMap.end();
		 i++)
		if (!*i)
			*i = bus;
}

bool AddressDecoder::mapComponent(OEComponent *component, const string &value)
{
	OEAddressRanges ranges;
	
	if (!getAddressRanges(ranges, value))
		return false;
	
	int shiftMask = (1 << shift) - 1;
	
	for (OEAddressRanges::iterator i = ranges.begin();
		 i != ranges.end();
		 i++)
	{
		if ((i->end > mask) ||
			(i->start & shiftMask) ||
			((i->end & shiftMask) != shiftMask))
		{
			OELog("address range " + value + " invalid");
			return false;
		}
		
		int startPage = i->start >> shift;
		int endPage = i->end >> shift;
		
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

bool AddressDecoder::getAddressRange(OEAddressRange &range, const string &value)
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
	else if (separatorPos == pos)
		return false;
	else if (separatorPos == value.size())
		return false;
	else
	{
		range.start = getInt(value.substr(pos, separatorPos));
		range.end = getInt(value.substr(separatorPos + 1));
	}
	
	if (range.start > range.end)
		return false;
	
	return true;
}

bool AddressDecoder::getAddressRanges(OEAddressRanges &ranges, const string &value)
{
	size_t startPos = value.find_first_not_of(',', 0);
	size_t endPos = value.find_first_of(',', startPos);
	
	while ((startPos != string::npos) || (endPos != string::npos))
	{
		OEAddressRange range;
		
		if (!getAddressRange(range, value.substr(startPos, endPos - startPos)))
		{
			OELog("address range '" + value + "' invalid");
			return false;
		}
		
		ranges.push_back(range);
		
		startPos = value.find_first_not_of(',', endPos);
		endPos = value.find_first_of(',', startPos);
	}
	
	return true;
}

bool AddressDecoder::setProperty(const string &name, const string &value)
{
	if (name == "addressSize")
	{
		int size = 1 << getInt(value);
		mask = size - 1;
		readMap.resize(size);
		writeMap.resize(size);
	}
	else if (name == "blockSize")
		shift = getInt(value);
	else if (name.substr(0, 3) == "map")
		componentMap[name.substr(3)] = value;
	else
		return false;
	
	return true;
}

bool AddressDecoder::connect(const string &name, OEComponent *component)
{
	if (name == "bus")
		mapBus(component);
	else if (name.substr(0, 9) == "component")
		mapComponent(component, componentMap[name.substr(9)]);
	else
		return false;
	
	return true;
}

bool AddressDecoder::postEvent(OEComponent *component, int notification, void *data)
{
	if (notification == ADDRESSDECODER_MAP)
	{
		if (component == NULL)
			component = bus;
		
		mapComponent(component, *((string *) data));
	}
	else
		return false;
	
	return true;
}

OEUInt8 AddressDecoder::read(int address)
{
	return readMap[(address & mask) >> shift]->read(address);
}

void AddressDecoder::write(int address, OEUInt8 value)
{
	writeMap[(address & mask) >> shift]->write(address, value);
}

