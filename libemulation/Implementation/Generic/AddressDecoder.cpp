
/**
 * libemulation
 * Address decoder
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
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
	
	for (MemoryMapsConf::iterator i = conf.begin();
		 i != conf.end();
		 i++)
	{
		if (!conf.count(i->first))
		{
			logMessage("invalid address range '" + i->first + "'");
            
			return false;
		}
        
        OEComponent *component = ref[i->first];
        
        if (!component)
            component = floatingBus;
        
		if (!appendMemoryMaps(memoryMaps, component, i->second))
			return false;
	}
    
	OEAddress addressSpace = (1 << addressSize);
	addressMask = addressSpace - 1;
	
	size_t blockNum = (size_t) (1 << (addressSize - blockSize));
	
    readMap.resize(blockNum);
	writeMap.resize(blockNum);
    
    readMapp = &readMap.front();
    writeMapp = &writeMap.front();
    
    refresh(0, addressMask);
	
	return true;
}

bool AddressDecoder::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case ADDRESSDECODER_MAP_MEMORYMAPS:
            return addMemoryMaps((MemoryMaps *) data);
            
        case ADDRESSDECODER_UNMAP_MEMORYMAPS:
            return removeMemoryMaps((MemoryMaps *) data);
	}
	
	return false;
}

OEUInt8 AddressDecoder::read(OEAddress address)
{
	return readMapp[(size_t) ((address & addressMask) >> blockSize)]->read(address);
//	return readMap[(size_t) ((address & addressMask) >> blockSize)]->read(address);
}

void AddressDecoder::write(OEAddress address, OEUInt8 value)
{
	writeMap[(size_t) ((address & addressMask) >> blockSize)]->write(address, value);
}

void AddressDecoder::mapMemory(MemoryMap& value)
{
	size_t startBlock = (size_t) (value.startAddress >> blockSize);
	size_t endBlock = (size_t) (value.endAddress >> blockSize);
	
    OEComponent *component = value.component;
    
    if (!component)
        component = floatingBus;
    
	if (value.read)
    {
        for (size_t i = startBlock; i <= endBlock; i++)
            readMap[i] = component;
    }
    
	if (value.write)
    {
        for (size_t i = startBlock; i <= endBlock; i++)
            writeMap[i] = component;
    }
}

void AddressDecoder::refresh(OEAddress startAddress, OEAddress endAddress)
{
    MemoryMap m;
    
    m.component = floatingBus;
    m.startAddress = startAddress;
    m.endAddress = endAddress;
    m.read = true;
    m.write = true;
    
    mapMemory(m);
    
    for (MemoryMaps::iterator i = memoryMaps.begin();
         i != memoryMaps.end();
         i++)
    {
        if ((i->endAddress < startAddress) ||
            (i->startAddress > endAddress))
            continue;
        
        m = *i;
        if (i->startAddress < startAddress)
            m.startAddress = startAddress;
        if (i->endAddress > endAddress)
            m.endAddress = endAddress;
        
        mapMemory(m);
    }
}

bool AddressDecoder::addMemoryMaps(MemoryMaps *value)
{
    for (MemoryMaps::iterator i = value->begin();
         i != value->end();
         i++)
        memoryMaps.push_back(*i);
        
    for (MemoryMaps::iterator i = value->begin();
         i != value->end();
         i++)
    {
        if (readMap.size())
            mapMemory(*i);
    }
    
    return true;
}

bool AddressDecoder::removeMemoryMaps(MemoryMaps *value)
{
    for (MemoryMaps::iterator i = value->begin();
         i != value->end();
         i++)
    {
        for (MemoryMaps::iterator j = memoryMaps.begin();
             j != memoryMaps.end();
             j++)
        {
            if ((i->component == j->component) &&
                (i->startAddress == j->startAddress) &&
                (i->endAddress == j->endAddress) &&
                (i->read == j->read) &&
                (i->write == j->write))
            {
                memoryMaps.erase(i);
                
                refresh(j->startAddress, j->endAddress);
                
                return true;
            }
        }
    }
    
    return false;
}
