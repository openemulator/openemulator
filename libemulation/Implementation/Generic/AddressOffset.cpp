
/**
 * libemulation
 * Address offset
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address offset
 */

#include "AddressOffset.h"
#include "AddressDecoder.h"

AddressOffset::AddressOffset()
{
    memory = NULL;
    
    offset = 0;
}

bool AddressOffset::setValue(string name, string value)
{
    if (name == "offset")
        offset = getOELong(value);
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
    
    return true;
}

bool AddressOffset::postMessage(OEComponent *sender, int message, void *data)
{
    if (message == ADDRESSOFFSET_SETOFFSET)
        offset = *((OEAddress *)data);
    else
        return false;
    
    return true;
}

OEChar AddressOffset::read(OEAddress address)
{
    return memory->read(address + offset);
}

void AddressOffset::write(OEAddress address, OEChar value)
{
    memory->write(address + offset, value);
}
