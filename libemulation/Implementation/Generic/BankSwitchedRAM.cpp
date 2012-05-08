

/**
 * libemulation
 * Bank Switched RAM
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls bank-switched random access memory
 */

#include "BankSwitchedRAM.h"

BankSwitchedRAM::BankSwitchedRAM() : RAM()
{
    blockSize = 0;
    
    offsetp = NULL;
}

bool BankSwitchedRAM::setValue(string name, string value)
{
    if (name == "blockSize")
        blockSize = getOEInt(value);
    else
        return RAM::setValue(name, value);
    
    return true;
}

bool BankSwitchedRAM::init()
{
    if (!RAM::init())
        return false;
    
    if ((blockSize != getNextPowerOf2(blockSize)) ||
        (size < blockSize) ||
        (!blockSize) ||
        (!size))
    {
		logMessage("invalid value for size/blockSize");
        
		return false;
    }
    
    blockBits = getBitNum(blockSize);
    
    offset.resize((size_t) (size / blockSize));
    offsetp = &offset.front();
    
    for (BankSwitchedRAMMaps::iterator i = ramMaps.begin();
         i != ramMaps.end();
         i++)
        mapMemory(*i);
    
    return true;
}

bool BankSwitchedRAM::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case BANKSWITCHEDRAM_MAP:
            return mapMemory(*((BankSwitchedRAMMap *) data));
            
        default:
            return RAM::postMessage(sender, message, data);
    }
}

OEChar BankSwitchedRAM::read(OEAddress address)
{
    address &= mask;
    
    return datap[address + offsetp[address >> blockBits]];
}

void BankSwitchedRAM::write(OEAddress address, OEChar value)
{
    address &= mask;
    
    datap[address + offsetp[address >> blockBits]] = value;
}

bool BankSwitchedRAM::mapMemory(BankSwitchedRAMMap& value)
{
    if (!offset.size())
        ramMaps.push_back(value);
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
