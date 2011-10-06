
/**
 * libemulation
 * RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls random access memory
 */

#include "RAM.h"
#include "AddressDecoder.h"

RAM::RAM()
{
    size = 0;
    
    controlBus = NULL;
    powerState = CONTROLBUS_POWERSTATE_ON;
}

bool RAM::setValue(string name, string value)
{
    if (name == "size")
        size = getUInt(value);
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
    if (name == "image")
        data->swap(this->data);
    else
        return false;
    
    return true;
}

bool RAM::getData(string name, OEData **data)
{
    if (name == "image")
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
    if (size <= 0)
    {
        logMessage("invalid RAM size");
        
        return false;
    }
    
    if (controlBus)
        controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    if (!powerOnPattern.size())
        powerOnPattern.resize(1);
    else
        powerOnPattern.resize((size_t) getNextPowerOf2((int) powerOnPattern.size()));
    
    size = getNextPowerOf2(size);
    size_t oldSize = data.size();
    data.resize((size_t) size);
    if (oldSize == 0)
        initMemory();
    datap = (OEUInt8 *) &data.front();
    mask = size - 1;
    
    return true;
}

bool RAM::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case RAM_GET_MEMORYSIZE:
            *((OEUInt64 *) data) = size;
            return true;
            
        case RAM_GET_MEMORY:
            *((OEData **) data) = &this->data;
            return true;
            
        case RAM_SET_MEMORY:
            this->data = *((OEData *) data);
            this->data.resize(size);
            return true;
    }
    
    return false;
}

void RAM::notify(OEComponent *sender, int notification, void *data)
{
    ControlBusPowerState oldPowerState = powerState;
    powerState = *((ControlBusPowerState *)data);
    
    if ((oldPowerState == CONTROLBUS_POWERSTATE_OFF) &&
        (powerState != CONTROLBUS_POWERSTATE_OFF))
        initMemory();
}

OEUInt8 RAM::read(OEAddress address)
{
    return datap[address & mask];
}

void RAM::write(OEAddress address, OEUInt8 value)
{
    datap[address & mask] = value;
}

void RAM::initMemory()
{
    int mask = (int) powerOnPattern.size() - 1;
    
    for (int i = 0; i < this->data.size(); i++)
        data[i] = powerOnPattern[i & mask];
}
