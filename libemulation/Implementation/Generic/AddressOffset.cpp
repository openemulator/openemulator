
/**
 * libemulation
 * Address offset
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address offseter
 */

#include "AddressOffset.h"

AddressOffset::AddressOffset()
{
    memory = NULL;
    
    size = 0;
    blockSize = 0;
    
    offsetp = NULL;
}

bool AddressOffset::setValue(string name, string value)
{
    if (name == "size")
        size = getOEInt(value);
    else if (name == "blockSize")
        blockSize = getOEInt(value);
    else
        return false;
    
    return true;
}

bool AddressOffset::setRef(string name, OEComponent *ref)
{
    if (name == "memory")
        memory = ref;
    else
        return false;
    
    return true;
}

bool AddressOffset::init()
{
    if (!memory)
    {
        logMessage("memory not connected");
        
        return false;
    }
    
    if ((blockSize != getNextPowerOf2(blockSize)) ||
        (size < blockSize) ||
        (!blockSize) ||
        (!size))
    {
		logMessage("invalid value for size/blockSize");
        
		return false;
    }
    
	mask = size - 1;
    blockBits = getBitNum(blockSize);
    
    offset.resize((size_t) (size / blockSize));
    offsetp = &offset.front();
    
    for (AddressOffsetMaps::iterator i = offsetMaps.begin();
         i != offsetMaps.end();
         i++)
        mapOffset(*i);
    
    return true;
}

bool AddressOffset::postMessage(OEComponent *sender, int message, void *data)
{
    if (message == ADDRESSOFFSET_MAP)
        return mapOffset(*((AddressOffsetMap *) data));
    else
        return false;
    
    return true;
}

OEChar AddressOffset::read(OEAddress address)
{
    return memory->read(address + offsetp[(address & mask) >> blockBits]);
}

void AddressOffset::write(OEAddress address, OEChar value)
{
    memory->write(address + offsetp[(address & mask) >> blockBits], value);
}

bool AddressOffset::mapOffset(AddressOffsetMap& value)
{
    if (!offset.size())
        offsetMaps.push_back(value);
    else
    {
        size_t startBlock = (size_t) (value.startAddress >> blockBits);
        size_t endBlock = (size_t) (value.endAddress >> blockBits);
        
        OESLong offset = value.offset;
        
        for (size_t i = startBlock; i <= endBlock; i++)
            offsetp[i] = offset;
    }
    
    return true;
}
