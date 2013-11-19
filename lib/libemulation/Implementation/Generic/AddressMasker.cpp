
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
    
    andMask = ~0;
    orMask = 0;
}

bool AddressMasker::setValue(string name, string value)
{
    if (name == "andMask")
        andMask = getOELong(value);
    else if (name == "orMask")
        orMask = getOELong(value);
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
    OECheckComponent(memory);
    
    return true;
}

bool AddressMasker::postMessage(OEComponent *sender, int message, void *data)
{
    if (message == ADDRESSMASKER_SET_ANDMASK)
        andMask = *((OEAddress *)data);
    else if (message == ADDRESSMASKER_SET_ORMASK)
        orMask = *((OEAddress *)data);
    else
        return false;
    
    return true;
}

OEChar AddressMasker::read(OEAddress address)
{
    return memory->read((address & andMask) | orMask);
}

void AddressMasker::write(OEAddress address, OEChar value)
{
    memory->write((address & andMask) | orMask, value);
}
