
/**
 * libemulation
 * Apple III RTC
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple III RTC
 */

#include "AppleIIIRTC.h"

#include "AppleIIIInterface.h"

AppleIIIRTC::AppleIIIRTC()
{
    systemControl = NULL;
    mm58167 = NULL;
}

bool AppleIIIRTC::setRef(string name, OEComponent *ref)
{
    if (name == "systemControl")
    {
        if (systemControl)
            systemControl->removeObserver(this, APPLEIII_ZEROPAGE_DID_CHANGE);
        systemControl = ref;
        if (systemControl)
            systemControl->addObserver(this, APPLEIII_ZEROPAGE_DID_CHANGE);
    }
    else if (name == "mm58167")
        mm58167 = ref;
    else
        return false;
    
    return true;
}

bool AppleIIIRTC::init()
{
    OECheckComponent(systemControl);
    OECheckComponent(mm58167);
    
    systemControl->postMessage(this, APPLEIII_GET_ZEROPAGE, &zeroPage);
    
    return true;
}

void AppleIIIRTC::notify(OEComponent *sender, int notification, void *data)
{
    zeroPage = *((OEChar *)data);
}

OEChar AppleIIIRTC::read(OEAddress address)
{
    return mm58167->read(zeroPage);
}

void AppleIIIRTC::write(OEAddress address, OEChar value)
{
    mm58167->write(zeroPage, value);
}
