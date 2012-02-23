
/**
 * libemulation
 * Address decoder
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address decoder
 */

#include <iostream>
#include <cctype>
#include <algorithm>

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
        confMap[name.substr(3)] = value;
	else
		return false;
	
	return true;
}

bool AddressDecoder::setRef(string name, OEComponent *ref)
{
	if (name == "floatingBus")
		floatingBus = ref;
	else if (name.substr(0, 3) == "ref")
		confRef[name.substr(3)] = ref;
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
    
	for (size_t i = 0; i < readMap.size(); i++)
	{
		readMap[i] = floatingBus;
		writeMap[i] = floatingBus;
	}
    
	for (AddressDecoderMap::iterator i = confMap.begin();
		 i != confMap.end();
		 i++)
	{
		if (!confRef.count(i->first))
		{
			logMessage("invalid address range '" + i->first + "'");
            
			return false;
		}
        
        OEComponent *component = confRef[i->first];
        
        if (!component)
            component = floatingBus;
        
		if (!mapConf(component, i->second))
			return false;
	}
    
    for (AddressDecoderMaps::iterator i = pendingMaps.begin();
         i != pendingMaps.end();
         i++)
		mapMemory(&(*i));
	
	return true;
}

bool AddressDecoder::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case ADDRESSDECODER_MAP_MEMORY:
            if (readMap.size())
                mapMemory((MemoryMap *) data);
            else
                pendingMaps.push_back(*((MemoryMap *) data));
            
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

bool AddressDecoder::getMemoryMap(MemoryMap& decoderMap,
                                  OEComponent *component,
                                  string value)
{
	decoderMap.component = component;
	
    value = strtolower(value);
    
	decoderMap.read = (value.find_first_of('r') != string::npos);
	decoderMap.write = (value.find_first_of('w') != string::npos);
    
	if (!decoderMap.read && !decoderMap.write)
		decoderMap.read = decoderMap.write = true;
	
    value = strfilter(value, "0123456789abcdef-x");
	
    vector<string> items = strsplit(value, '-');
    
    if (items.size() == 1)
    {
        if (items[0] == "")
            return false;
        
        decoderMap.startAddress = decoderMap.endAddress = getUInt(items[0]);
    }
    else if (items.size() == 2)
    {
        if ((items[0] == "") || (items[1] == ""))
            return false;
        
        decoderMap.startAddress = getUInt(items[0]);
        decoderMap.endAddress = getUInt(items[1]);
    }
    else
        return false;
    
    OEAddress blockMask = (1 << blockSize) - 1;
    
    if ((decoderMap.startAddress > decoderMap.endAddress) ||
        (decoderMap.endAddress > addressMask) ||
        (decoderMap.startAddress & blockMask) ||
        ((decoderMap.endAddress & blockMask) != blockMask))
        return false;
    
	return true;
}

void AddressDecoder::mapMemory(MemoryMap *theMap)
{
	size_t startBlock = (size_t) (theMap->startAddress >> blockSize);
	size_t endBlock = (size_t) (theMap->endAddress >> blockSize);
	
    OEComponent *component = theMap->component;
    
    if (!component)
        component = floatingBus;
    
	if (theMap->read)
    {
        for (size_t i = startBlock; i <= endBlock; i++)
            readMap[i] = component;
    }
    
	if (theMap->write)
    {
        for (size_t i = startBlock; i <= endBlock; i++)
            writeMap[i] = component;
    }
}

bool AddressDecoder::mapConf(OEComponent *component, string value)
{
    vector<string> items = strsplit(value, ',');
    
    for (vector<string>::iterator i = items.begin();
         i != items.end();
         i++)
    {
		MemoryMap memoryMap;
        
		if (!getMemoryMap(memoryMap, component, *i))
		{
			logMessage("invalid map '" + value + "'");
            
			return false;
		}
		
        mapMemory(&memoryMap);
    };
    
    return true;
}
