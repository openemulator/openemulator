
/**
 * libemulator
 * Apple II Slot Controller
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot memory ($C100-$C7FF)
 */

#include "AppleIISlotController.h"

#include "MemoryInterface.h"

#include "AppleIIInterface.h"

AppleIISlotController::AppleIISlotController()
{
    memory = NULL;
    memoryBus = NULL;
    
    en = false;
}

bool AppleIISlotController::setValue(string name, string value)
{
	if (name == "en")
		en = getOEInt(value);
	else if (name.substr(0, 3) == "map")
        conf[name.substr(3)] = value;
	else
		return false;
    
	return true;
}

bool AppleIISlotController::getValue(string name, string& value)
{
	if (name == "en")
        value = getString(en);
	else
		return false;
    
    return true;
}

bool AppleIISlotController::setRef(string name, OEComponent *ref)
{
	if (name == "memory")
		memory = ref;
	else if (name == "memoryBus")
		memoryBus = ref;
	else if (name.substr(0, 3) == "ref")
		this->ref[name.substr(3)] = ref;
	else
		return false;
	
	return true;
}

bool AppleIISlotController::init()
{
    OECheckComponent(memoryBus);
    OECheckComponent(memory);
    
    bool success = true;
    
	for (MemoryMapsConf::iterator i = conf.begin();
		 i != conf.end();
		 i++)
	{
		if (!ref.count(i->first))
		{
			logMessage("undeclared ref for address map '" + i->first + "'");
            
            success = false;
            
			continue;
		}
        
        OEComponent *component = ref[i->first];
        
        if (!component)
            continue;
        
		appendMemoryMaps(memoryMaps, component, i->second);
    }
    
    if (en)
        updateSlotExpansion(true);
    
    return true;
}

void AppleIISlotController::dispose()
{
    if (en)
        updateSlotExpansion(false);
}

bool AppleIISlotController::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case APPLEII_DISABLE_C800:
            updateSlotExpansion(false);
            
            return true;
            
        case APPLEII_IS_C800_ENABLED:
            *((bool *)data) = en;
            
            return true;
    }
    
    return false;
}

void AppleIISlotController::notify(OEComponent *sender, int notification, void *data)
{
    enableSlotExpansion(false);
}

OEChar AppleIISlotController::read(OEAddress address)
{
    enableSlotExpansion(true);
    
    return memory->read(address);
}

void AppleIISlotController::write(OEAddress address, OEChar value)
{
    enableSlotExpansion(true);
    
    memory->write(address, value);
}

void AppleIISlotController::enableSlotExpansion(bool value)
{
    if (en == value)
        return;
    
    updateSlotExpansion(value);
}

void AppleIISlotController::updateSlotExpansion(bool value)
{
    en = value;
    
    int message = en ? APPLEII_MAP_SLOT : APPLEII_UNMAP_SLOT;
    
    for (MemoryMaps::iterator i = memoryMaps.begin();
         i != memoryMaps.end();
         i++)
        memoryBus->postMessage(this, message, &*i);
    
    postNotification(this, APPLEII_C800_DID_CHANGE, &en);
}
