
/**
 * libemulation
 * Mux
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Multiplexes several components.
 */

#include "AddressMux.h"

AddressMux::AddressMux()
{
}

bool AddressMux::setValue(string name, string value)
{
    if (name == "sel")
        sel = value;
    else
        return false;
    
    return true;
}

bool AddressMux::getValue(string name, string& value)
{
    if (name == "sel")
        value = sel;
    else
        return false;
    
    return true;
}

bool AddressMux::setRef(string name, OEComponent *ref)
{
    if (name.substr(0, 3) == "ref")
        this->ref[name.substr(3)] = ref;
    else
        return false;
    
    return true;
}

void AddressMux::update()
{
    if (ref.count(sel))
        component = ref[sel];
    else
        component = &dummyComponent;
}

bool AddressMux::postMessage(OEComponent *sender, int message, void *data)
{
    return component->postMessage(sender, message, data);
}

OEUInt8 AddressMux::read(OEAddress address)
{
    return component->read(address);
}

void AddressMux::write(OEAddress address, OEUInt8 value)
{
    component->write(address, value);
}

OEUInt16 AddressMux::read16(OEAddress address)
{
    return component->read16(address);
}

void AddressMux::write16(OEAddress address, OEUInt16 value)
{
    component->write16(address, value);
}

OEUInt32 AddressMux::read32(OEAddress address)
{
    return component->read32(address);
}

void AddressMux::write32(OEAddress address, OEUInt32 value)
{
    component->write32(address, value);
}

OEUInt64 AddressMux::read64(OEAddress address)
{
    return component->read64(address);
}

void AddressMux::write64(OEAddress address, OEUInt64 value)
{
    component->write64(address, value);
}
