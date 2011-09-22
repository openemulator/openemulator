
/**
 * libemulation
 * Mux
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Multiplexes several components.
 */

#include "Mux.h"

Mux::Mux()
{
    component = NULL;
}

bool Mux::setValue(string name, string value)
{
    if (name == "sel")
        sel = value;
    else
        return false;
    
    return true;
}

bool Mux::getValue(string name, string& value)
{
    if (name == "sel")
        value = sel;
    else
        return false;
    
    return true;
}

bool Mux::setRef(string name, OEComponent *ref)
{
	if (name.substr(0, 3) == "ref")
		this->ref[name.substr(3)] = ref;
    else
        return false;
    
    return true;
}

void Mux::update()
{
    if (ref.count(sel))
        component = ref[sel];
    else
        component = NULL;
}

bool Mux::postMessage(OEComponent *sender, int message, void *data)
{
    if (component)
        return component->postMessage(sender, message, data);
    
    return false;
}

OEUInt8 Mux::read(OEAddress address)
{
    if (component)
        return component->read(address);
    
    return 0;
}

void Mux::write(OEAddress address, OEUInt8 value)
{
    if (component)
        component->write(address, value);
}

OEUInt16 Mux::read16(OEAddress address)
{
    if (component)
        return component->read16(address);
    
    return 0;
}

void Mux::write16(OEAddress address, OEUInt16 value)
{
    if (component)
        component->write16(address, value);
}

OEUInt32 Mux::read32(OEAddress address)
{
    if (component)
        return component->read32(address);
    
    return 0;
}

void Mux::write32(OEAddress address, OEUInt32 value)
{
    if (component)
        component->write32(address, value);
}

OEUInt64 Mux::read64(OEAddress address)
{
    if (component)
        return component->read64(address);
    
    return 0;
}

void Mux::write64(OEAddress address, OEUInt64 value)
{
    if (component)
        component->write64(address, value);
}
