
/**
 * libemulator
 * Apple II Floating Bus
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the Apple II floating bus
 */

#include "AppleIIFloatingBus.h"

#include "AppleIIInterface.h"

AppleIIFloatingBus::AppleIIFloatingBus()
{
    video = NULL;
}

bool AppleIIFloatingBus::setRef(string name, OEComponent *ref)
{
	if (name == "video")
		video = ref;
	else
		return false;
	
	return true;
}

bool AppleIIFloatingBus::init()
{
    if (!video)
    {
        logMessage("video not connected");
        
        return false;
    }
    
    return true;
}

OEUInt8 AppleIIFloatingBus::read(OEAddress address)
{
    OEUInt8 value;
    
    video->postMessage(this, APPLEIIVIDEO_READ_FLOATINGBUS, &value);
    
	return value;
}
