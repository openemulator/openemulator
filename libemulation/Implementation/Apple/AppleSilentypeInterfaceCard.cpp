
/**
 * libemulation
 * Apple Silentype Interface Card
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Silentype Interface Card
 */

#include "AppleSilentypeInterfaceCard.h"

#include "ControlBusInterface.h"
#include "AppleIIInterface.h"

AppleSilentypeInterfaceCard::AppleSilentypeInterfaceCard()
{
    controlBus = NULL;
    floatingBus = NULL;
    
    lastCycles = 0;
}

bool AppleSilentypeInterfaceCard::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
        controlBus = ref;
    else if (name == "floatingBus")
        floatingBus = ref;
    else
        return false;
    
    return true;
}

bool AppleSilentypeInterfaceCard::init()
{
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    return true;
}

bool AppleSilentypeInterfaceCard::postMessage(OEComponent *sender, int message, void *data)
{
    return false;
}

OEChar AppleSilentypeInterfaceCard::read(OEAddress address)
{
    return 0;
}

void AppleSilentypeInterfaceCard::write(OEAddress address, OEChar value)
{
}
