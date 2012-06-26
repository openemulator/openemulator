
/**
 * libemulation
 * Video RAM
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls video random access memory
 */

#include "VRAM.h"

#include "MemoryInterface.h"

VRAM::VRAM() : RAM()
{
    observer = NULL;
}

bool VRAM::setRef(string name, OEComponent *ref)
{
    if (name == "observer")
        observer = ref;
    else
        return RAM::setRef(name, ref);
    
    return true;
}

bool VRAM::init()
{
    if (!RAM::init())
        return false;
    
    if (!observer)
    {
		logMessage("observer not connected");
        
		return false;
    }
    
    return true;
}

void VRAM::write(OEAddress address, OEChar value)
{
    observer->notify(this, VIDEORAM_WILL_CHANGE, NULL);
    
    datap[address & mask] = value;
}
