
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
	size = 0;
	blockSize = 0;
    
	floatingBus = NULL;
	
	mask = 0;
}

bool AddressDecoder::setValue(string name, string value)
{
	if (name == "size")
		size = getOEInt(value);
	else if (name == "blockSize")
		blockSize = getOEInt(value);
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
	
    if ((size != getNextPowerOf2(size)) ||
        (blockSize != getNextPowerOf2(blockSize)) ||
        (size < blockSize) ||
        (!blockSize) ||
        (!size))
    {
		logMessage("invalid value for size/blockSize");
        
		return false;
    }
    
	mask = size - 1;
    blockBits = getBitNum(blockSize);
	
	size_t blockNum = (size_t) (size / blockSize);
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
		case ADDRESSDECODER_MAP:
            return addMemoryMap((MemoryMap *) data);
            
        case ADDRESSDECODER_UNMAP:
            return removeMemoryMap((MemoryMap *) data);
	}
	
	return false;
}

OEChar AddressDecoder::read(OEAddress address)
{
	return readMapp[(size_t) ((address & mask) >> blockBits)]->read(address);
}

void AddressDecoder::write(OEAddress address, OEChar value)
{
	writeMapp[(size_t) ((address & mask) >> blockBits)]->write(address, value);
}

void AddressDecoder::mapMemory(MemoryMap& value)
{
	size_t startBlock = (size_t) (value.startAddress >> blockBits);
	size_t endBlock = (size_t) (value.endAddress >> blockBits);
	
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
    
    updateMemoryMaps(0, mask);
    
    return success;
}

bool AddressDecoder::addMemoryMap(MemoryMap *value)
{
    if (!value->component)
        return false;
    
    dynamicMemoryMaps.push_back(*value);
    
    if (readMap.size())
        mapMemory(*value);
    
    return true;
}

bool AddressDecoder::removeMemoryMap(MemoryMap *value)
{
    for (MemoryMaps::iterator i = dynamicMemoryMaps.begin();
         i != dynamicMemoryMaps.end();
         i++)
    {
        if ((i->component == value->component) &&
            (i->startAddress == value->startAddress) &&
            (i->endAddress == value->endAddress) &&
            (i->read == value->read) &&
            (i->write == value->write))
        {
            i = dynamicMemoryMaps.erase(i);
            
            updateMemoryMaps(value->startAddress, value->endAddress);
        }
    }
    
    return true;
}
