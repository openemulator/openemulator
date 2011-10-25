
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
        
        OEComponent *component = ref[i->first];
        
        if (!component)
            component = floatingBus;
        
		if (!mapConf(component, i->second))
			return false;
	}
    
    // Store defaults
    defaultReadMap = readMap;
    defaultWriteMap = writeMap;
    
    // Map pending decoder maps
    for (AddressDecoderMaps::iterator i = pendingMaps.begin();
         i != pendingMaps.end();
         i++)
		mapDecoderMap(&(*i));
	
	return true;
}

bool AddressDecoder::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case ADDRESSDECODER_MAP:
            if (readMap.size())
                mapDecoderMap((AddressDecoderMap *) data);
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

void AddressDecoder::mapDecoderMap(AddressDecoderMap *theMap)
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

bool AddressDecoder::getDecoderMap(AddressDecoderMap& decoderMap,
                                   OEComponent *component,
                                   string confItem)
{
	decoderMap.component = component;
	
    confItem = strtolower(confItem);
    
	decoderMap.read = (confItem.find_first_of('r') != string::npos);
	decoderMap.write = (confItem.find_first_of('w') != string::npos);
    
	if (!decoderMap.read && !decoderMap.write)
		decoderMap.read = decoderMap.write = true;
	
    confItem = strfilter(confItem, "0123456789abcdef-x");
	
    vector<string> item = strsplit(confItem, '-');
    
    if (item.size() == 1)
    {
        if (item[0] == "")
            return false;
        
        decoderMap.startAddress = decoderMap.endAddress = getUInt(item[0]);
    }
    else if (item.size() == 2)
    {
        if ((item[0] == "") || (item[1] == ""))
            return false;
        
        decoderMap.startAddress = getUInt(item[0]);
        decoderMap.endAddress = getUInt(item[1]);
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

bool AddressDecoder::mapConf(OEComponent *component, string conf)
{
    vector<string> confItem = strsplit(conf, ',');
    
    for (vector<string>::iterator i = confItem.begin();
         i != confItem.end();
         i++)
    {
		AddressDecoderMap theMap;
        
		if (!getDecoderMap(theMap, component, *i))
		{
			logMessage("invalid map '" + conf + "'");
            
			return false;
		}
		
        mapDecoderMap(&theMap);
    };
    
    return true;
}
