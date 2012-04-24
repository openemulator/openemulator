
/**
 * libemulation
 * ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls read only memory
 */

#include "ROM.h"

bool ROM::setData(string name, OEData *data)
{
    if (name == "memoryImage")
        data->swap(this->data);
    else
        return false;
    
    return true;
}

bool ROM::getData(string name, OEData **data)
{
    if (name == "memoryImage")
        *data = &this->data;
    else
        return false;
    
    return true;
}

bool ROM::init()
{
    if (!data.size())
    {
        logMessage("missing ROM");
        
        return false;
    }
    
    OEAddress size = getNextPowerOf2((int) data.size());
    data.resize((size_t) size);
    datap = &data.front();
    mask = size - 1;
    
    return true;
}

OEChar ROM::read(OEAddress address)
{
    return datap[address & mask];
}
