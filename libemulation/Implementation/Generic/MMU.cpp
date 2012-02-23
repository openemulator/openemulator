

/**
 * libemulation
 * MMU
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic MMU
 */

#include <iostream>
#include <cctype>
#include <algorithm>

#include "MMU.h"

MMU::MMU()
{
    addressDecoder = NULL;
}

bool MMU::setValue(string name, string value)
{
	if (name.substr(0, 3) == "map")
		confMap[name.substr(3)] = value;
	else
		return false;
	
	return true;
}

bool MMU::setRef(string name, OEComponent *ref)
{
    if (name == "addressDecoder")
        addressDecoder = ref;
	else if (name.substr(0, 3) == "ref")
		confRef[name.substr(3)] = ref;
	else
		return false;
	
	return true;
}

bool MMU::init()
{
    if (!addressDecoder)
    {
        logMessage("addressDecoder not connected");
        
        return false;
    }
    
	for (MMUMap::iterator i = confMap.begin();
		 i != confMap.end();
		 i++)
	{
		if (!confRef.count(i->first))
		{
			logMessage("invalid address range '" + i->first + "'");
            
			return false;
		}
        
 		if (!mapConf(confRef[i->first], i->second))
			return false;
	}
    
    // Set maps
    for (MemoryMaps::iterator i = memoryMaps.begin();
         i != memoryMaps.end();
         i++)
        addressDecoder->postMessage(this, ADDRESSDECODER_MAP_MEMORY, &(*i));
    
	return true;
}

bool MMU::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case MMU_MAP_MEMORY:
            if (!addMemoryMap((MemoryMap *) data))
                return false;
            
			return true;
            
        case MMU_UNMAP_MEMORY:
            if (!removeMemoryMap((MemoryMap *) data))
                return false;
            
			return true;
	}
	
	return false;
}

bool MMU::getMemoryMap(MemoryMap& decoderMap,
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
    
	return true;
}

bool MMU::mapConf(OEComponent *component, string value)
{
    vector<string> confItem = strsplit(value, ',');
    
    for (vector<string>::iterator i = confItem.begin();
         i != confItem.end();
         i++)
    {
		MemoryMap memoryMap;
        
		if (!getMemoryMap(memoryMap, component, *i))
		{
			logMessage("invalid map '" + value + "'");
            
			return false;
		}
		
        addMemoryMap(&memoryMap);
    };
    
    return true;
}

bool MMU::addMemoryMap(MemoryMap *value)
{
    for (MemoryMaps::iterator i = memoryMaps.begin();
         i != memoryMaps.end();
         i++)
    {
        if ((i->component == value->component) &&
            (i->startAddress == value->startAddress) &&
            (i->endAddress == value->endAddress) &&
            (i->read == value->read) &&
            (i->write == value->write))
            return false;
    }
    
    memoryMaps.push_back(*value);
    
    addressDecoder->postMessage(this, ADDRESSDECODER_MAP_MEMORY, value);
    
    return true;
}

void MMU::unmap(MemoryMap *value)
{
    MemoryMap m = *value;
    m.component = NULL;
    addressDecoder->postMessage(this, ADDRESSDECODER_MAP_MEMORY, &m);
    
    for (MemoryMaps::iterator i = memoryMaps.begin();
         i != memoryMaps.end();
         i++)
    {
        if ((i->endAddress < value->startAddress) ||
            (i->startAddress > value->endAddress))
            continue;
        
        MemoryMap m = *i;
        if (i->startAddress < value->startAddress)
            m.startAddress = value->startAddress;
        if (i->endAddress > value->endAddress)
            m.endAddress = value->endAddress;
        
        addressDecoder->postMessage(this, ADDRESSDECODER_MAP_MEMORY, &m);
    }
}

bool MMU::removeMemoryMap(MemoryMap *value)
{
    for (MemoryMaps::iterator i = memoryMaps.begin();
         i != memoryMaps.end();
         i++)
    {
        if ((i->component == value->component) &&
            (i->startAddress == value->startAddress) &&
            (i->endAddress == value->endAddress) &&
            (i->read == value->read) &&
            (i->write == value->write))
        {
            memoryMaps.erase(i);
            
            unmap(value);
            
            return true;
        }
    }
    
    return false;
}
