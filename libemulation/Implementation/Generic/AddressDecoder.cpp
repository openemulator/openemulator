
/**
 * libemulation
 * Address decoder
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
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
}

bool AddressDecoder::setValue(string name, string value)
{
	if (name == "addressSize")
		addressSize = getUInt(value);
	else if (name == "blockSize")
		blockSize = getUInt(value);
	else if (name.substr(0, 3) == "map")
		conf[name.substr(3)] = value;
	else
		return false;
	
	return true;
}

bool AddressDecoder::setRef(string name, OEComponent *ref)
{
	if (name == "floatingBus")
		floatingBus = ref;
	else if (name.substr(0, 3) == "ref")
		this->ref[name.substr(3)] = ref;
	else
		return false;
	
	return true;
}

bool AddressDecoder::init()
{
	if (!floatingBus)
	{
		logMessage("floatingBus not connected");
		return false;
	}
	
	OEAddress addressSpace = (1 << addressSize);
	addressMask = addressSpace - 1;
	
	size_t blockNum = (size_t) (1 << (addressSize - blockSize));
	
    readMap.resize(blockNum);
	writeMap.resize(blockNum);
    
    defaultReadMap.resize(blockNum);
    defaultWriteMap.resize(blockNum);
	
	for (size_t i = 0; i < blockNum; i++)
	{
		readMap[i] = floatingBus;
		writeMap[i] = floatingBus;
	}
	
    // Map configuration
	for (AddressDecoderConf::iterator i = conf.begin();
		 i != conf.end();
		 i++)
	{
		if (!ref.count(i->first))
		{
			logMessage("invalid address range '" + i->first + "'");
			return false;
		}
        
        if (!ref[i->first])
        {
			logMessage("'" + i->first + "' not connected");
            return false;
        }
		
		if (!mapRange(ref[i->first], i->second))
			return false;
	}
    
    // Store defaults
    defaultReadMap = readMap;
    defaultWriteMap = writeMap;
    
    // Map pending ranges
    for (AddressDecoderMaps::iterator i = pendingMaps.begin();
         i != pendingMaps.end();
         i++)
		mapRange(&(*i));
	
	return true;
}

bool AddressDecoder::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case ADDRESSDECODER_MAP:
            if (readMap.size())
                mapRange((AddressDecoderMap *) data);
            else
                pendingMaps.push_back(*((AddressDecoderMap *) data));
            
			return true;
	}
	
	return false;
}

OEUInt8 AddressDecoder::read(OEAddress address)
{
	return readMap[(size_t) ((address & addressMask) >> blockSize)]->read(address);
}

void AddressDecoder::write(OEAddress address, OEUInt8 value)
{
	writeMap[(size_t) ((address & addressMask) >> blockSize)]->write(address, value);
}

void AddressDecoder::mapRange(AddressDecoderMap *theMap)
{
	size_t startBlock = (size_t) (theMap->startAddress >> blockSize);
	size_t endBlock = (size_t) (theMap->endAddress >> blockSize);
	
	if (theMap->read)
    {
        if (theMap->component)
            for (size_t i = startBlock; i <= endBlock; i++)
                readMap[i] = theMap->component;
        else
            for (size_t i = startBlock; i <= endBlock; i++)
                readMap[i] = defaultReadMap[i];
    }
    
	if (theMap->write)
    {
        if (theMap->component)
            for (size_t i = startBlock; i <= endBlock; i++)
                writeMap[i] = theMap->component;
        else
            for (size_t i = startBlock; i <= endBlock; i++)
                writeMap[i] = defaultWriteMap[i];
    }
}

bool AddressDecoder::mapRange(OEComponent *component, string value)
{
	AddressDecoderMaps maps;
	
	if (!getMaps(maps, component, value))
		return false;
	
	OEAddress blockMask = (1 << blockSize) - 1;
	
	for (AddressDecoderMaps::iterator i = maps.begin();
		 i != maps.end();
		 i++)
	{
		if ((i->startAddress > i->endAddress) ||
			(i->endAddress > addressMask) ||
			(i->startAddress & blockMask) ||
			((i->endAddress & blockMask) != blockMask))
		{
			logMessage("invalid address range '" + value + "'");
			return false;
		}
		
		mapRange(&(*i));
	}
	
	return true;
}

bool AddressDecoder::getMaps(AddressDecoderMaps& theMaps,
                             OEComponent *component,
							 string value)
{
	size_t startPos = value.find_first_not_of(',', 0);
	size_t endPos = value.find_first_of(',', startPos);
	
	while ((startPos != string::npos) || (endPos != string::npos))
	{
		AddressDecoderMap theMap;
		
		if (!getMap(theMap, component, value.substr(startPos, endPos - startPos)))
		{
			logMessage("invalid address range '" + value + "'");
			return false;
		}
		
		theMaps.push_back(theMap);
		
		startPos = value.find_first_not_of(',', endPos);
		endPos = value.find_first_of(',', startPos);
	}
	
	return true;
}

bool AddressDecoder::getMap(AddressDecoderMap& theMap,
                            OEComponent *component,
							string value)
{
	theMap.component = component;
	
	theMap.read = false;
	theMap.write = false;
	
	size_t pos = 0;
	
	while(1)
	{
		if (pos == value.size())
			return false;
		else if (tolower(value[pos]) == 'r')
			theMap.read = true;
		else if (tolower(value[pos]) == 'w')
			theMap.write = true;
		else
			break;
        
        pos++;
	}
	
	if (!theMap.read && !theMap.write)
		theMap.read = theMap.write = true;
	
	size_t separatorPos = value.find_first_of('-', pos);
	if (separatorPos == string::npos)
		theMap.endAddress = theMap.startAddress = getUInt(value.substr(pos));
	else if (separatorPos == pos)
		return false;
	else if (separatorPos == value.size())
		return false;
	else
	{
		theMap.startAddress = getUInt(value.substr(pos, separatorPos));
		theMap.endAddress = getUInt(value.substr(separatorPos + 1));
	}
	
	return true;
}
