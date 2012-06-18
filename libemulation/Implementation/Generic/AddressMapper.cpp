
/**
 * libemulation
 * Address mapper
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Maps components in an address decoder.
 */

#include "AddressMapper.h"

AddressMapper::AddressMapper()
{
}

bool AddressMapper::setValue(string name, string value)
{
    if (name == "sel")
        sel = value;
	else if (name.substr(0, 3) == "map")
        conf[name.substr(3)] = value;
    else
        return false;
    
    return true;
}

bool AddressMapper::getValue(string name, string& value)
{
    if (name == "sel")
        value = sel;
    else
        return false;
    
    return true;
}

bool AddressMapper::setRef(string name, OEComponent *ref)
{
    if (name == "addressDecoder")
        addressDecoder = ref;
	else if (name.substr(0, 3) == "ref")
		this->ref[name.substr(3)] = ref;
    else
        return false;
    
    return true;
}

bool AddressMapper::init()
{
    if (!addressDecoder)
    {
        logMessage("addressDecoder not connected");
        
        return false;
    }
    
    update();
    
    return true;
}

void AddressMapper::update()
{
    if (lastSel == sel)
        return;
    
    MemoryMaps m;
    
    if (ref.count(lastSel))
    {
        if (!appendMemoryMaps(m, ref[lastSel], conf[lastSel]))
            logMessage("could not remove memory map " + conf[lastSel] + " for " + lastSel);
        
        for (MemoryMaps::iterator i = m.begin();
             i != m.end();
             i++)
            addressDecoder->postMessage(this, ADDRESSDECODER_UNMAP, &*i);
    }
    
    m.clear();
    
    if (ref.count(sel))
    {
        if (!appendMemoryMaps(m, ref[sel], conf[sel]))
            logMessage("could not add memory map '" + conf[sel] + "' for '" + sel + "'");
        
        for (MemoryMaps::iterator i = m.begin();
             i != m.end();
             i++)
            addressDecoder->postMessage(this, ADDRESSDECODER_MAP, &*i);
    }
    
    lastSel = sel;
}

bool AddressMapper::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case ADDRESSMAPPER_MAP:
            sel = *((string *)data);
            
            update();
            
            return true;
    }
    
    return false;
}
