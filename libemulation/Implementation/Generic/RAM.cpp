
/**
 * libemulation
 * RAM
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls random access memory
 */

#include "RAM.h"

#include "MemoryInterface.h"

RAM::RAM()
{
    size = 0;
    
    controlBus = NULL;
    powerState = CONTROLBUS_POWERSTATE_ON;
}

bool RAM::setValue(string name, string value)
{
    if (name == "size")
        size = getOELong(value);
    else if (name == "powerOnPattern")
        powerOnPattern = getCharVector(value);
    else
        return false;
    
    return true;
}

bool RAM::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
    }
    else
        return false;
    
    return true;
}

bool RAM::setData(string name, OEData *data)
{
    if (name == "memoryImage")
        data->swap(this->data);
    else
        return false;
    
    return true;
}

bool RAM::getData(string name, OEData **data)
{
    if (name == "memoryImage")
    {
        if (powerState == CONTROLBUS_POWERSTATE_OFF)
            *data = NULL;
        else
            *data = &this->data;
    }
    else
        return false;
    
    return true;
}

bool RAM::init()
{
    if ((size != getNextPowerOf2(size)) ||
        (!size))
    {
		logMessage("invalid value for size");
        
		return false;
    }
    
    if (controlBus)
        controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    if (!powerOnPattern.size())
        powerOnPattern.resize(1);
    else
        powerOnPattern.resize((size_t) getNextPowerOf2((int) powerOnPattern.size()));
    
    size_t oldSize = data.size();
    data.resize((size_t) size);
    if (oldSize == 0)
        initMemory();
    datap = &data.front();
    mask = size - 1;
    
    return true;
}

bool RAM::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case RAM_GET_DATA:
            *((OEData **) data) = &this->data;
            return true;
    }
    
    return false;
}

void RAM::notify(OEComponent *sender, int notification, void *data)
{
    powerState = *((ControlBusPowerState *)data);
    
    if (powerState == CONTROLBUS_POWERSTATE_OFF)
        initMemory();
}

OEChar RAM::read(OEAddress address)
{
    return datap[address & mask];
}

void RAM::write(OEAddress address, OEChar value)
{
    datap[address & mask] = value;
}

void RAM::initMemory()
{
    int mask = (int) powerOnPattern.size() - 1;
    
    for (int i = 0; i < this->data.size(); i++)
        data[i] = powerOnPattern[i & mask];
}
