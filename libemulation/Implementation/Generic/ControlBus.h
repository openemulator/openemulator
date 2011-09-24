
/**
 * libemulation
 * Control bus
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a control bus with clock control and reset/IRQ/NMI lines
 */

#ifndef _CONTROLBUS_H
#define _CONTROLBUS_H

#include "OEComponent.h"

#include "ControlBusInterface.h"

#include <list>

typedef struct
{
    OEUInt64 cycles;
    OEComponent *component;
} ControlBusEvent;

class ControlBus : public OEComponent
{
public:
    ControlBus();
    
    bool setValue(string name, string value);
    bool getValue(string name, string &value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int event, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    float clockFrequency;
    float cpuClockMultiplier;
    bool resetOnPowerOn;
    
    OEComponent *device;
    OEComponent *audio;
    OEComponent *cpu;
    
    ControlBusPowerState powerState;
    
    OEUInt32 resetCount;
    OEUInt32 irqCount;
    OEUInt32 nmiCount;
    
    OEUInt64 cycleCount;
    OEUInt64 cycleStart;
    float sampleToCycleRatio;
    
    bool inBlock;
    float blockSize;
    OEInt32 blockCPUCycles;
    float blockOffset;
    
    list<ControlBusEvent> events;
    
    void setPowerState(ControlBusPowerState powerState);
    
    OEUInt64 getCycleCount();
    void scheduleTimer(OEComponent *component, OEUInt64 cycles);
    void clearTimers(OEComponent *component);
    
    void setCPUClockMultiplier(float cpuClockMultiplier);
};

#endif
