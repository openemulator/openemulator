
/**
 * libemulator
 * Apple II Video Sync
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Synchronizes video with memory
 */

#include "AppleIIVideoSync.h"

#include "AppleIIInterface.h"

AppleIIVideoSync::AppleIIVideoSync()
{
    video = NULL;
    ram = NULL;
}

bool AppleIIVideoSync::setRef(string name, OEComponent *ref)
{
    if (name == "video")
        video = ref;
    else
        return false;
    
    return true;
}

bool AppleIIVideoSync::init()
{
    if (!video)
    {
        logMessage("video not connected");
        
        return false;
    }
    
    return true;
}

void AppleIIVideoSync::write(OEAddress address, OEUInt8 value)
{
    video->postMessage(this, APPLEIIVIDEO_REFRESH, NULL);
    
    ram->write(address, value);
}
