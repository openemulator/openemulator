
/**
 * libemulation
 * Control bus
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a control bus with clock control and reset/IRQ/NMI lines
 */

#ifndef _CONTROLBUS_H
#define _CONTROLBUS_H

#include <list>

#include "OEComponent.h"

#include "ControlBusInterface.h"

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
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int event, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    float clockFrequency;
    float cpuClockMultiplier;
    ControlBusPowerState powerState;
    bool resetOnPowerOn;
    OEUInt32 resetCount;
    OEUInt32 irqCount;
    OEUInt32 nmiCount;
    
    OEComponent *device;
    OEComponent *audio;
    OEComponent *cpu;
    
    OEUInt64 cycles;
    float cpuCycles;
    list<ControlBusEvent> events;
    bool inEvent;
    
    OEUInt64 audioBufferStart;
    float sampleToCycleRatio;
    
    void setPowerState(ControlBusPowerState value);
    
    OEInt64 getPendingCPUCycles();
    void setPendingCPUCycles(OEInt64 value);
    void runCPU();
    OEUInt64 getCycles();
    void scheduleTimer(OEComponent *component, OEUInt64 cycles);
    void invalidateTimers(OEComponent *component);
    
    void setCPUClockMultiplier(float value);
};

#endif
