
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
	addressSize = 16;
	blockSize = 8;
	floatingBus = NULL;
	
	addressMask = 0;
	
	readMap.resize(1);
	writeMap.resize(1);
}

bool AddressDecoder::setValue(const string &name, const string &value)
{
	if (name == "addressSize")
		addressSize = getInt(value);
	else if (name == "blockSize")
		blockSize = getInt(value);
	else if (name.substr(0, 3) == "map")
		addressMap[name.substr(3)] = value;
	else
		return false;
	
	return true;
}

bool AddressDecoder::setComponent(const string &name, OEComponent *component)
{
	if (name == "floatingBus")
		floatingBus = component;
	else if (name.substr(0, 3) == "ref")
		addressRef[name.substr(3)] = component;
	else
		return false;
	
	return true;
}

bool AddressDecoder::init()
{
	if (!floatingBus)
	{
		OELog("undefined floating bus");
		return false;
	}
	
	int addressSpace = (1 << addressSize);
	addressMask = addressSpace - 1;
	
	int blockNum = 1 << (addressSize - blockSize);
	readMap.resize(blockNum);
	writeMap.resize(blockNum);
	
	initMap(floatingBus);
	
	for (AddressMap::iterator i = addressMap.begin();
		 i != addressMap.end();
		 i++)
	{
		if (!addressRef.count(i->first))
		{
			OELog("unmatched address map '" + i->first + "'");
			return false;
		}
		
		mapComponent(addressRef[i->first], i->second);
	}
	
	return true;
}

bool AddressDecoder::postEvent(OEComponent *component, int notification, void *data)
{
	if (notification == ADDRESSDECODER_MAP)
	{
		if (component == NULL)
			component = floatingBus;
		
		mapComponent(component, *((string *) data));
	}
	else
		return false;
	
	return true;
}

OEUInt8 AddressDecoder::read(int address)
{
	return readMap[(address & addressMask) >> blockSize]->read(address);
}

void AddressDecoder::write(int address, OEUInt8 value)
{
	writeMap[(address & addressMask) >> blockSize]->write(address, value);
}

void AddressDecoder::initMap(OEComponent *component)
{
	for (OEComponents::iterator i = readMap.begin();
		 i != readMap.end();
		 i++)
		*i = component;
	
	for (OEComponents::iterator i = writeMap.begin();
		 i != writeMap.end();
		 i++)
		*i = component;
}

void AddressDecoder::mapComponent(AddressRange *range)
{
	int startBlock = range->start >> blockSize;
	int endBlock = range->end >> blockSize;
	
	if (range->read)
		for (int j = startBlock; j < endBlock; j++)
			readMap[j] = range->component;

	if (range->write)
		for (int j = startBlock; j < endBlock; j++)
			writeMap[j] = range->component;
}

bool AddressDecoder::mapComponent(OEComponent *component, const string &value)
{
	AddressRanges ranges;
	
	if (!getAddressRanges(ranges, value))
		return false;
	
	int shiftMask = (1 << addressSize) - 1;
	
	for (AddressRanges::iterator i = ranges.begin();
		 i != ranges.end();
		 i++)
	{
		if ((i->end > addressMask) ||
			(i->start & shiftMask) ||
			((i->end & shiftMask) != shiftMask))
		{
			OELog("address range " + value + " invalid");
			return false;
		}
		
		mapComponent(&(*i));
	}
	
	return true;
}

bool AddressDecoder::getAddressRange(AddressRange &range, const string &value)
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

bool AddressDecoder::getAddressDecoderMaps(AddressDecoderMaps &theMaps, const string &value)
{
	size_t startPos = value.find_first_not_of(',', 0);
	size_t endPos = value.find_first_of(',', startPos);
	
	while ((startPos != string::npos) || (endPos != string::npos))
	{
		AddressDecoderMap theMap;
		
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
