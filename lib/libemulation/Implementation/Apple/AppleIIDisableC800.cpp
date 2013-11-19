
/**
 * libemulator
 * Apple II Disable C800
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Disables Apple II slot expansion memory ($C800-$CFFF)
 */

#include "AppleIIDisableC800.h"

#include "AppleIIInterface.h"

AppleIIDisableC800::AppleIIDisableC800()
{
    memory = NULL;
    memoryBus = NULL;
}

bool AppleIIDisableC800::setRef(string name, OEComponent *ref)
{
	if (name == "memory")
		memory = ref;
	else if (name == "memoryBus")
		memoryBus = ref;
	else
		return false;
	
	return true;
}

bool AppleIIDisableC800::init()
{
    OECheckComponent(memory);
    OECheckComponent(memoryBus);
    
    return true;
}

OEChar AppleIIDisableC800::read(OEAddress address)
{
    memory->postMessage(this, APPLEII_DISABLE_C800, NULL);
    
    // Re-read to disable other cards if necessary
    return memoryBus->read(address);
}

void AppleIIDisableC800::write(OEAddress address, OEChar value)
{
    memory->postMessage(this, APPLEII_DISABLE_C800, NULL);
    
    // Re-write to disable other cards if necessary
    return memoryBus->write(address, value);
}
