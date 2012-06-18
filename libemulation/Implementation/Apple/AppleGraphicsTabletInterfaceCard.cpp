
/**
 * libemulation
 * Apple Graphics Tablet Interface Card
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Graphics Tablet Interface Card
 */

#include "AppleGraphicsTabletInterfaceCard.h"

#include "ControlBusInterface.h"
#include "AppleIIInterface.h"

AppleGraphicsTabletInterfaceCard::AppleGraphicsTabletInterfaceCard()
{
    graphicsTablet = NULL;
    controlBus = NULL;
    floatingBus = NULL;
    memory = NULL;
    
    proximity = false;
    x = 0.5;
    y = 0.5;
    button = false;
    
    count = 0;
    
    timerEnabled = false;
    timerCount = 0;
    timerCycles = 0;
}

bool AppleGraphicsTabletInterfaceCard::setRef(string name, OEComponent *ref)
{
    if (name == "graphicsTablet")
    {
        if (graphicsTablet)
            graphicsTablet->removeObserver(this, CANVAS_POINTER_DID_CHANGE);
        graphicsTablet = ref;
        if (graphicsTablet)
            graphicsTablet->addObserver(this, CANVAS_POINTER_DID_CHANGE);
    }
    else if (name == "controlBus")
        controlBus = ref;
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "memory")
    {
        if (memory)
            memory->removeObserver(this, APPLEII_C800_DID_CHANGE);
        memory = ref;
        if (memory)
            memory->addObserver(this, APPLEII_C800_DID_CHANGE);
    }
    else if (name == "audioCodec")
        audioCodec = ref;
    else
        return false;
    
    return true;
}

bool AppleGraphicsTabletInterfaceCard::init()
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
    
    if (!memory)
    {
        logMessage("memory not connected");
        
        return false;
    }
    
    memory->postMessage(this, APPLEII_IS_C800_ENABLED, &timerEnabled);
    
    return true;
}

void AppleGraphicsTabletInterfaceCard::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == graphicsTablet)
    {
        CanvasHIDEvent *event = (CanvasHIDEvent *) data;
        
        switch (event->usageId)
        {
            case CANVAS_P_PROXIMITY:
                proximity = event->value;
                
                break;
                
            case CANVAS_P_X:
                x = event->value;
                if (x < 0)
                    x = 0;
                else if (x > 1)
                    x = 1;
                
                break;
                
            case CANVAS_P_Y:
                y = event->value;
                if (y < 0)
                    y = 0;
                else if (y > 1)
                    y = 1;
                
                break;
                
            case CANVAS_P_BUTTON1:
                button = event->value;
                
                break;
        }
    }
    else if (sender == memory)
    {
        updateCount();
        
        // C800 did change
        timerEnabled = *((bool *) data);
        
        if (!timerEnabled)
            count &= 0xf00;
    }
}

OEChar AppleGraphicsTabletInterfaceCard::read(OEAddress address)
{
    updateCount();
    
    switch (address & 0x3)
    {
        case 0:
        {
            // X Drive enabled
            if (proximity)
                setTimer(2150 * x + 300);
            
            count |= 0xf00;
            
            return floatingBus->read(address);
        }
        case 1:
        {
            // Y Drive enabled
            if (proximity)
                setTimer(2150 * y + 300);
            
            count |= 0xf00;
            
            return floatingBus->read(address);
        }
        case 2:
            // Read counter (lowest 4 bits)
            return (floatingBus->read(address) & 0xf0) | ((count & 0xf) ^ 0x7);
            
        case 3:
            // Read counter (highest 8 bits)
            count = (count & 0xff0) | !button;
            
            return (count >> 4);
    }
    
    return 0;
}

void AppleGraphicsTabletInterfaceCard::write(OEAddress address, OEChar value)
{
    read(address);
}

void AppleGraphicsTabletInterfaceCard::setTimer(OEInt value)
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    timerCycles = cycles;
    timerCount = value;
}

void AppleGraphicsTabletInterfaceCard::updateCount()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    // 7 MHz count clock with 7x LC-tuned frequency multiplier
    OELong delta = 7 * 7 * (cycles - timerCycles);
    
    if (delta > timerCount)
        delta = timerCount;
    
    timerCount -= (OEInt) delta;
    timerCycles = cycles;
    
    if (timerEnabled)
    {
        count += (OEInt) delta;
        count &= 0xfff;
    }
}
