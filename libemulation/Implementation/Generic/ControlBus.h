
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
    OELong cycles;
    OEComponent *component;
    OEInt id;
} ControlBusEvent;

class ControlBus : public OEComponent
{
public:
    ControlBus();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void dispose();
    
    bool postMessage(OEComponent *sender, int event, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    float clockFrequency;
    float cpuClockMultiplier;
    ControlBusPowerState powerState;
    bool resetOnPowerOn;
    OEInt resetCount;
    OEInt irqCount;
    OEInt nmiCount;
    
    OEComponent *device;
    OEComponent *audio;
    OEComponent *cpu;
    
    OELong cycles;
    float cpuCycles;
    list<ControlBusEvent> events;
    bool inEvent;
    
    OELong audioBufferStart;
    float sampleToCycleRatio;
    
    bool activity;
    
    void setPowerState(ControlBusPowerState value);
    void updatePowerState();
    
    void setActivity(bool value);
    void updateActivity();
    
    OESLong getPendingCPUCycles();
    void setPendingCPUCycles(OESLong value);
    void runCPU();
    OELong getCycles();
    void scheduleTimer(OEComponent *component, OELong cycles, OEInt id);
    void invalidateTimers(OEComponent *component, OEInt id);
    
    void setCPUClockMultiplier(float value);
};

#endif
