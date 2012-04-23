
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
	
	OEAddress addressSpace = (1 << addressSize);
	addressMask = addressSpace - 1;
	
	size_t blockNum = (size_t) (1 << (addressSize - blockSize));
	
    readMap.resize(blockNum);
	writeMap.resize(blockNum);
    
    readMapp = &readMap.front();
    writeMapp = &writeMap.front();
    
    return updateMemoryMaps();
}

void AddressDecoder::update()
{
    updateMemoryMaps();
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
}

void AddressDecoder::write(OEAddress address, OEUInt8 value)
{
	writeMapp[(size_t) ((address & addressMask) >> blockSize)]->write(address, value);
}

void AddressDecoder::mapMemory(MemoryMap& value)
{
	size_t startBlock = (size_t) (value.startAddress >> blockSize);
	size_t endBlock = (size_t) (value.endAddress >> blockSize);
	
    OEComponent *component = value.component;
    
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

void AddressDecoder::updateMemoryMaps(MemoryMaps& value,
                                      OEAddress startAddress,
                                      OEAddress endAddress)
{
    for (MemoryMaps::iterator i = value.begin();
         i != value.end();
         i++)
    {
        if ((i->endAddress < startAddress) ||
            (i->startAddress > endAddress))
            continue;
        
        MemoryMap m = *i;
        if (i->startAddress < startAddress)
            m.startAddress = startAddress;
        if (i->endAddress > endAddress)
            m.endAddress = endAddress;
        
        mapMemory(m);
    }
}

void AddressDecoder::updateMemoryMaps(OEAddress startAddress, OEAddress endAddress)
{
    MemoryMap m;
    
    m.component = floatingBus;
    m.startAddress = startAddress;
    m.endAddress = endAddress;
    m.read = true;
    m.write = true;
    
    mapMemory(m);
    
    updateMemoryMaps(staticMemoryMaps, startAddress, endAddress);
    
    updateMemoryMaps(dynamicMemoryMaps, startAddress, endAddress);
}

bool AddressDecoder::updateMemoryMaps()
{
    bool success = true;
    
    staticMemoryMaps.clear();
    
	for (MemoryMapsConf::iterator i = conf.begin();
		 i != conf.end();
		 i++)
	{
		if (!ref.count(i->first))
		{
			logMessage("undeclared ref for address map '" + i->first + "'");
            
            success = false;
            
			continue;
		}
        
        OEComponent *component = ref[i->first];
        
        if (!component)
            continue;
        
		appendMemoryMaps(staticMemoryMaps, component, i->second);
	}
    
    updateMemoryMaps(0, addressMask);
    
    return success;
}

bool AddressDecoder::addMemoryMaps(MemoryMaps *value)
{
    for (MemoryMaps::iterator i = value->begin();
         i != value->end();
         i++)
    {
        if (!i->component)
            continue;
        
        dynamicMemoryMaps.push_back(*i);
    }
    
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
        for (MemoryMaps::iterator j = dynamicMemoryMaps.begin();
             j != dynamicMemoryMaps.end();
             j++)
        {
            if ((i->component == j->component) &&
                (i->startAddress == j->startAddress) &&
                (i->endAddress == j->endAddress) &&
                (i->read == j->read) &&
                (i->write == j->write))
            {
                dynamicMemoryMaps.erase(i);
                
                updateMemoryMaps(j->startAddress, j->endAddress);
                
                return true;
            }
        }
    }
    
    return false;
}
