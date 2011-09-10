
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

#include <list>

typedef enum
{
    CONTROLBUS_SET_POWERSTATE,
    CONTROLBUS_GET_POWERSTATE,
    
    CONTROLBUS_ASSERT_RESET,
    CONTROLBUS_CLEAR_RESET,
    CONTROLBUS_ASSERT_IRQ,
    CONTROLBUS_CLEAR_IRQ,
    CONTROLBUS_ASSERT_NMI,
    CONTROLBUS_CLEAR_NMI,
    CONTROLBUS_IS_RESET_ASSERTED,
    CONTROLBUS_IS_IRQ_ASSERTED,
    CONTROLBUS_IS_NMI_ASSERTED,
    
    CONTROLBUS_GET_CYCLECOUNT,
    CONTROLBUS_GET_AUDIOBUFFERINDEX,
    
    CONTROLBUS_SCHEDULE_TIMER,
    CONTROLBUS_CLEAR_TIMERS,
    
    CONTROLBUS_SET_CPUCLOCKMULTIPLIER,
} ControlBusMessage;

typedef enum
{
    CONTROLBUS_POWERSTATE_DID_CHANGE,
    
    CONTROLBUS_RESET_DID_ASSERT,
    CONTROLBUS_RESET_DID_CLEAR,
    CONTROLBUS_IRQ_DID_ASSERT,
    CONTROLBUS_IRQ_DID_CLEAR,
    CONTROLBUS_NMI_DID_ASSERT,
    CONTROLBUS_NMI_DID_CLEAR,
    
    CONTROLBUS_TIMER_DID_FIRE,
} ControlBusNotification;

typedef enum
{
    CONTROLBUS_POWERSTATE_ON,
    CONTROLBUS_POWERSTATE_PAUSE,
    CONTROLBUS_POWERSTATE_STANDBY,
    CONTROLBUS_POWERSTATE_SLEEP,
    CONTROLBUS_POWERSTATE_HIBERNATE,
    CONTROLBUS_POWERSTATE_OFF,
} ControlBusPowerState;

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
