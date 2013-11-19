
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
	
    readMapp = NULL;
    writeMapp = NULL;
    
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
    OECheckComponent(floatingBus);
	
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
    
    if (!updateInternalMemoryMaps())
        return false;
    
    updateReadWriteMap(0, mask);
    
    return true;
}

void AddressDecoder::update()
{
    if (!updateInternalMemoryMaps())
        return;
    
    updateReadWriteMap(0, mask);
}

bool AddressDecoder::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case ADDRESSDECODER_MAP:
            return addMemoryMap(externalMemoryMaps, (MemoryMap *) data);
        
        case ADDRESSDECODER_UNMAP:
            return removeMemoryMap(externalMemoryMaps, (MemoryMap *) data);
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

void AddressDecoder::updateReadWriteMap(MemoryMaps& value,
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

void AddressDecoder::updateReadWriteMap(OEAddress startAddress, OEAddress endAddress)
{
    updateReadWriteMap(internalMemoryMaps, startAddress, endAddress);
    updateReadWriteMap(externalMemoryMaps, startAddress, endAddress);
}

bool AddressDecoder::updateInternalMemoryMaps()
{
    bool success = true;
    
    internalMemoryMaps.clear();
    
    MemoryMap m;
    
    m.component = floatingBus;
    m.startAddress = 0;
    m.endAddress = mask;
    m.read = true;
    m.write = true;
    
    internalMemoryMaps.push_back(m);
    
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
        
		appendMemoryMaps(internalMemoryMaps, component, i->second);
	}
    
    return success;
}

bool AddressDecoder::addMemoryMap(MemoryMaps& maps, MemoryMap *value)
{
    maps.push_back(*value);
    
    if (readMapp)
        updateReadWriteMap(value->startAddress, value->endAddress);
    
    return true;
}

bool AddressDecoder::removeMemoryMap(MemoryMaps& maps, MemoryMap *value)
{
    for (MemoryMaps::iterator i = maps.begin();
         i != maps.end();
         i++)
    {
        if ((i->component == value->component) &&
            (i->startAddress == value->startAddress) &&
            (i->endAddress == value->endAddress) &&
            (i->read == value->read) &&
            (i->write == value->write))
        {
            i = maps.erase(i);
            
            if (readMapp)
                updateReadWriteMap(value->startAddress, value->endAddress);
        }
    }
    
    return true;
}
