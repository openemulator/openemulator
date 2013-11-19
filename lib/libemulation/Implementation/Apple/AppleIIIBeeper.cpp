
/**
 * libemulation
 * Apple III Beeper
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple III Beeper
 */

#include "AppleIIIBeeper.h"

#include "ControlBusInterface.h"

#include "AppleIIInterface.h"

#define DURATION    0.11F
#define FREQUENCY   953

AppleIIIBeeper::AppleIIIBeeper()
{
    controlBus = NULL;
    floatingBus = NULL;
    audioOut = NULL;
    
    count = 0;
}

bool AppleIIIBeeper::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
        controlBus = ref;
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "audioOut")
        audioOut = ref;
    else
        return false;
    
    return true;
}

bool AppleIIIBeeper::init()
{
    OECheckComponent(controlBus);
    OECheckComponent(floatingBus);
    OECheckComponent(audioOut);
    
    return true;
}

void AppleIIIBeeper::notify(OEComponent *sender, int notification, void *data)
{
    audioOut->write(0xc030, 0);
    
    count--;
    
    if (count)
        scheduleNextTimer(*((OESLong *)data));
}

OEChar AppleIIIBeeper::read(OEAddress address)
{
    write(address, 0);
    
    return floatingBus->read(address);
}

void AppleIIIBeeper::write(OEAddress address, OEChar value)
{
    if (!count)
        scheduleNextTimer(0);
    
    count = 2.0F * DURATION * FREQUENCY;
}

void AppleIIIBeeper::scheduleNextTimer(OESLong cycles)
{
    ControlBusTimer timer = { cycles + 0.5F * APPLEII_CLOCKFREQUENCY / FREQUENCY, 0};
    
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &timer);
}
