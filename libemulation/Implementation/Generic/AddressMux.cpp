
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
    component = NULL;
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
		confRef[name.substr(3)] = ref;
    else
        return false;
    
    return true;
}

void AddressMux::update()
{
    if (confRef.count(sel))
        component = confRef[sel];
    else
        component = NULL;
}

bool AddressMux::postMessage(OEComponent *sender, int message, void *data)
{
    if (component)
        return component->postMessage(sender, message, data);
    
    return false;
}

OEUInt8 AddressMux::read(OEAddress address)
{
    if (component)
        return component->read(address);
    
    return 0;
}

void AddressMux::write(OEAddress address, OEUInt8 value)
{
    if (component)
        component->write(address, value);
}

OEUInt16 AddressMux::read16(OEAddress address)
{
    if (component)
        return component->read16(address);
    
    return 0;
}

void AddressMux::write16(OEAddress address, OEUInt16 value)
{
    if (component)
        component->write16(address, value);
}

OEUInt32 AddressMux::read32(OEAddress address)
{
    if (component)
        return component->read32(address);
    
    return 0;
}

void AddressMux::write32(OEAddress address, OEUInt32 value)
{
    if (component)
        component->write32(address, value);
}

OEUInt64 AddressMux::read64(OEAddress address)
{
    if (component)
        return component->read64(address);
    
    return 0;
}

void AddressMux::write64(OEAddress address, OEUInt64 value)
{
    if (component)
        component->write64(address, value);
}
