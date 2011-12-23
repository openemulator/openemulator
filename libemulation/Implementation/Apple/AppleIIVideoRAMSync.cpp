
/**
 * libemulator
 * Apple II Video RAM Sync
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Synchronizes video with video RAM
 */

#include "AppleIIVideoRAMSync.h"

#include "AppleIIInterface.h"

AppleIIVideoRAMSync::AppleIIVideoRAMSync()
{
    video = NULL;
    ram1 = NULL;
    ram2 = NULL;
}

bool AppleIIVideoRAMSync::setRef(string name, OEComponent *ref)
{
    if (name == "video")
        video = ref;
    else if (name == "ram1")
        ram1 = ref;
    else if (name == "ram2")
        ram2 = ref;
    else
        return false;
    
    return true;
}

bool AppleIIVideoRAMSync::init()
{
    if (!video)
    {
        logMessage("video not connected");
        
        return false;
    }
    
    return true;
}

void AppleIIVideoRAMSync::write(OEAddress address, OEUInt8 value)
{
    video->postMessage(this, APPLEIIVIDEO_REFRESH, NULL);
    
    if (!(address & 0x4000))
        ram1->write(address, value);
    else
        ram2->write(address, value);
}
