
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
#include "HostStorageInterface.h"

AddressDecoder::AddressDecoder()
{
	hostStorage = NULL;
	
	addressSize = 16;
	blockSize = 8;
	floatingBus = NULL;
	
	addressMask = 0;
	
	readMap.resize(1);
	writeMap.resize(1);
}

bool AddressDecoder::setValue(string name, string value)
{
	if (name == "addressSize")
		addressSize = getInt(value);
	else if (name == "blockSize")
		blockSize = getInt(value);
	else if (name.substr(0, 3) == "map")
		conf[name.substr(3)] = value;
	else
		return false;
	
	return true;
}

bool AddressDecoder::setRef(string name, OEComponent *id)
{
	if (name == "hostStorage")
	{
		replaceObserver(hostStorage, id, HOST_STORAGE_MOUNT_REQUESTED);
		hostStorage = id;
	}
	else if (name == "floatingBus")
		floatingBus = id;
	else if (name.substr(0, 3) == "ref")
		this->ref[name.substr(3)] = id;
	else
		return false;
	
	return true;
}

bool AddressDecoder::init()
{
	if (!floatingBus)
	{
		log("floating bus undefined");
		return false;
	}
	
	OEAddress addressSpace = (1 << addressSize);
	addressMask = addressSpace - 1;
	
	int blockNum = 1 << (addressSize - blockSize);
	readMap.resize(blockNum);
	writeMap.resize(blockNum);
	
	for (int i = 0; i < blockNum; i++)
	{
		readMap[i] = floatingBus;
		writeMap[i] = floatingBus;
	}
	
	for (AddressDecoderConf::iterator i = conf.begin();
		 i != conf.end();
		 i++)
	{
		if (!ref.count(i->first))
		{
			log("unmatched address conf '" + i->first + "'");
			return false;
		}
		
		if (!map(ref[i->first], i->second))
			return false;
	}
	
	return true;
}

bool AddressDecoder::postMessage(OEComponent *sender, int message, void *data)
{
	if (message == ADDRESSDECODER_MAP)
		map((AddressDecoderMap *) data);
	else
		return false;
	
	return true;
}

OEUInt8 AddressDecoder::read(OEAddress address)
{
	return readMap[(address & addressMask) >> blockSize]->read(address);
}

void AddressDecoder::write(OEAddress address, OEUInt8 value)
{
	writeMap[(address & addressMask) >> blockSize]->write(address, value);
}

void AddressDecoder::map(AddressDecoderMap *map)
{
	int startBlock = map->startAddress >> blockSize;
	int endBlock = map->endAddress >> blockSize;
	
	if (map->read)
		for (int j = startBlock; j < endBlock; j++)
			readMap[j] = map->component;

	if (map->write)
		for (int j = startBlock; j < endBlock; j++)
			writeMap[j] = map->component;
}

bool AddressDecoder::map(OEComponent *component, string value)
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
			log("address range " + value + " invalid");
			return false;
		}
		
		map(&(*i));
	}
	
	return true;
}

bool AddressDecoder::getMaps(AddressDecoderMaps &maps, OEComponent *component,
							 string value)
{
	size_t startPos = value.find_first_not_of(',', 0);
	size_t endPos = value.find_first_of(',', startPos);
	
	while ((startPos != string::npos) || (endPos != string::npos))
	{
		AddressDecoderMap map;
		
		if (!getMap(map, component, value.substr(startPos, endPos - startPos)))
		{
			log("address range '" + value + "' invalid");
			return false;
		}
		
		maps.push_back(map);
		
		startPos = value.find_first_not_of(',', endPos);
		endPos = value.find_first_of(',', startPos);
	}
	
	return true;
}

bool AddressDecoder::getMap(AddressDecoderMap &map, OEComponent *component,
							string value)
{
	map.component = component;
	
	map.read = false;
	map.write = false;
	
	size_t pos = 0;
	
	while(1)
	{
		if (pos == value.size())
			return false;
		else if (tolower(value[pos]) == 'r')
			map.read = true;
		else if (tolower(value[pos]) == 'w')
			map.write = true;
		else
			break;
	}
	
	if (!map.read && !map.write)
		map.read = map.write = true;
	
	size_t separatorPos = value.find_first_of('-', pos);
	if (separatorPos == string::npos)
		map.endAddress = map.startAddress = getInt(value.substr(pos));
	else if (separatorPos == pos)
		return false;
	else if (separatorPos == value.size())
		return false;
	else
	{
		map.startAddress = getInt(value.substr(pos, separatorPos));
		map.endAddress = getInt(value.substr(separatorPos + 1));
	}
	
	return true;
}
