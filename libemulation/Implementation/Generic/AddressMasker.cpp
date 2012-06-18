
/**
 * libemulation
 * Address masker
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address masker
 */

#include "AddressMasker.h"

#include "MemoryInterface.h"

AddressMasker::AddressMasker()
{
    memory = NULL;
    
    mask = ~0;
}

bool AddressMasker::setValue(string name, string value)
{
    if (name == "mask")
        mask = getOELong(value);
    else
        return false;
    
    return true;
}

bool AddressMasker::setRef(string name, OEComponent *ref)
{
    if (name == "memory")
        memory = ref;
    else
        return false;
    
    return true;
}

bool AddressMasker::init()
{
    if (!memory)
    {
        logMessage("memory not connected");
        
        return false;
    }
    
    return true;
}

bool AddressMasker::postMessage(OEComponent *sender, int message, void *data)
{
    if (message == ADDRESSMASKER_SET_MASK)
        mask = *((OEAddress *)data);
    else
        return false;
    
    return true;
}

OEChar AddressMasker::read(OEAddress address)
{
    return memory->read(address & mask);
}

void AddressMasker::write(OEAddress address, OEChar value)
{
    memory->write(address & mask, value);
}
