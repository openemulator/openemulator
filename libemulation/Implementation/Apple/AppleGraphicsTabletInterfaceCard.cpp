
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
    
    lastCycles = 0;
    
    countEnabled = false;
    count = 0;
    
    proximity = false;
    x = 0.5;
    y = 0.5;
    button = false;
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
    
    return true;
}

void AppleGraphicsTabletInterfaceCard::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == graphicsTablet)
    {
        switch (notification)
        {
            case CANVAS_P_PROXIMITY:
                proximity = *((bool *)data);
                
                break;
                
            case CANVAS_P_X:
                x = *((bool *)data);
                
                break;
                
            case CANVAS_P_Y:
                y = *((bool *)data);
                
                break;
                
            case CANVAS_P_BUTTON1:
                button = *((bool *)data);
                
                break;
        }
    }
    else
    {
        updateCount();
        
        countEnabled = *((bool *)data);
        count &= 0xf00;
    }
}

OEChar AppleGraphicsTabletInterfaceCard::read(OEAddress address)
{
    switch (address & 0x3)
    {
        case 0:
        {
            // X Drive enabled
            updateCount();
            
            bool proximity = false;
            float x = 0;
            
            if (proximity)
                setTimer(x);
            
            count |= 0xf00;
            
            return floatingBus->read(address);
        }
        case 1:
        {
            // Y Drive enabled
            updateCount();
            
            bool proximity = false;
            float y = 0;
            
            if (proximity)
                setTimer(y);
            
            count |= 0xf00;
            
            return floatingBus->read(address);
        }
        case 2:
            // Read 4-bit low counter
            updateCount();
            
            return (floatingBus->read(address) & 0xf0) | (~count & 0xf);
            
        case 3:
            // Read 8-bit high counter
            updateCount();
            
            bool button = false;
            
            count = (count & 0xff0) | button;
            
            return (count >> 4);
    }
    
    return 0;
}

void AppleGraphicsTabletInterfaceCard::write(OEAddress address, OEChar value)
{
    read(address);
}

void AppleGraphicsTabletInterfaceCard::setTimer(float value)
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    lastCycles = cycles + 4096 * value;
}

void AppleGraphicsTabletInterfaceCard::updateCount()
{
/*    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    // 7 MHz count clock
    OELong countDelta = 7 * (cycles - lastCycles);
    
    lastCycles = cycles;
    
    if (countDelta > 2000)
        countDelta = 2000;
    
    count += countDelta;*/
    
    count = 0x400;
}
