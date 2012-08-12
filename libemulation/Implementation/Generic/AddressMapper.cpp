
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
    OECheckComponent(addressDecoder);
    
    update();
    
    return true;
}

void AddressMapper::update()
{
    if (lastSel == sel)
        return;
    
    MemoryMaps m;
    
    m = buildMemoryMaps(lastSel);
    
    for (MemoryMaps::iterator i = m.begin();
         i != m.end();
         i++)
        addressDecoder->postMessage(this, ADDRESSDECODER_UNMAP, &*i);
    
    m = buildMemoryMaps(sel);
    
    for (MemoryMaps::iterator i = m.begin();
         i != m.end();
         i++)
        addressDecoder->postMessage(this, ADDRESSDECODER_MAP, &*i);
    
    lastSel = sel;
}

void AddressMapper::dispose()
{
    sel = "";
    
    update();
}

bool AddressMapper::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case ADDRESSMAPPER_SELECT:
            sel = *((string *)data);
            
            update();
            
            return true;
    }
    
    return false;
}

MemoryMaps AddressMapper::buildMemoryMaps(string value)
{
    MemoryMaps m;
    
    vector<string> values = strsplit(value, ',');
    
    for (vector<string>::iterator i = values.begin();
         i != values.end();
         i++)
    {
        string sel = *i;
        
        if (!ref.count(sel))
            continue;
        
        if (!appendMemoryMaps(m, ref[sel], conf[sel]))
            logMessage("could not remove memory map " + conf[lastSel] + " for " + lastSel);
    }
    
    return m;
}
