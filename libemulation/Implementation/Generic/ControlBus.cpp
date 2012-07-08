
/**
 * libemulation
 * Control bus
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a control bus with clock control and reset/IRQ/NMI lines
 */

#include <math.h>

#include "ControlBus.h"
#include "DeviceInterface.h"
#include "AudioInterface.h"
#include "CPUInterface.h"

ControlBus::ControlBus()
{
    clockFrequency = 1E6;
    cpuClockMultiplier = 1;
    powerState = CONTROLBUS_POWERSTATE_OFF;
    resetOnPowerOn = true;
    resetCount = 0;
    irqCount = 0;
    nmiCount = 0;
    
    device = NULL;
    audio = NULL;
    cpu = NULL;
    
    cycles = 0;
    cpuCycles = 0;
    inEvent = false;
    
    audioBufferStart = 0;
    sampleToCycleRatio = 0;
    
    activity = false;
}

bool ControlBus::setValue(string name, string value)
{
    if (name == "clockFrequency")
        clockFrequency = getFloat(value);
    else if (name == "cpuClockMultiplier")
        cpuClockMultiplier = getFloat(value);
    else if (name == "powerState")
    {
        if (value.substr(0, 1) == "S")
            powerState = (ControlBusPowerState) getOEInt(value.substr(1));
    }
    else if (name == "resetOnPowerOn")
        resetOnPowerOn = getOEInt(value);
    else if (name == "resetCount")
        resetCount = getOEInt(value);
    else if (name == "irqCount")
        irqCount = getOEInt(value);
    else if (name == "nmiCount")
        nmiCount = getOEInt(value);
    else
        return false;
    
    return true;
}

bool ControlBus::getValue(string name, string& value)
{
    if (name == "powerState")
        value = string("S") + getString(powerState);
    else if (name == "resetCount")
        value = getString(resetCount);
    else if (name == "irqCount")
        value = getString(irqCount);
    else if (name == "nmiCount")
        value = getString(nmiCount);
    else
        return false;
    
    return true;
}

bool ControlBus::setRef(string name, OEComponent *ref)
{
    if (name == "device")
    {
        if (device)
            device->removeObserver(this, DEVICE_DID_CHANGE);
        device = ref;
        if (device)
            device->addObserver(this, DEVICE_DID_CHANGE);
    }
    else if (name == "audio")
    {
        if (audio)
            audio->removeObserver(this, AUDIO_BUFFER_IS_RENDERING);
        audio = ref;
        if (audio)
            audio->addObserver(this, AUDIO_BUFFER_IS_RENDERING);
    }
    else if (name == "cpu")
        cpu = ref;
    else
        return false;
    
    return true;
}

bool ControlBus::init()
{
    if (!device)
    {
        logMessage("device not connected");
        
        return false;
    }
    
    if (!audio)
    {
        logMessage("audio not connected");
        
        return false;
    }
    
    if (!cpu)
    {
        logMessage("cpu not connected");
        
        return false;
    }
    
    updatePowerState();
    
    return true;
}

void ControlBus::dispose()
{
    setActivity(false);
}

bool ControlBus::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case CONTROLBUS_SET_POWERSTATE:
            setPowerState(*((ControlBusPowerState *)data));
            
            return true;
            
        case CONTROLBUS_GET_POWERSTATE:
            *((ControlBusPowerState *)data) = powerState;
            
            return true;
            
        case CONTROLBUS_SET_CLOCKFREQUENCY:
            clockFrequency = *((float *)data);
            
            postNotification(this, CONTROLBUS_CLOCKFREQUENCY_DID_CHANGE, &clockFrequency);
            
            return true;
            
        case CONTROLBUS_GET_CLOCKFREQUENCY:
            *((float *)data) = clockFrequency;
            
            return true;
            
        case CONTROLBUS_GET_CYCLES:
            *((OELong *)data) = cycles + getCycles();
            
            return true;
            
        case CONTROLBUS_GET_AUDIOBUFFERFRAME:
            *((float *)data) = (cycles + getCycles() - audioBufferStart) * sampleToCycleRatio;
            
            return true;
            
        case CONTROLBUS_SCHEDULE_TIMER:
            scheduleTimer(sender, *((OELong *)data));
            
            return true;
            
        case CONTROLBUS_INVALIDATE_TIMERS:
            invalidateTimers(sender);
            
            return true;
            
        case CONTROLBUS_SET_CPUCLOCKMULTIPLIER:
            setCPUClockMultiplier(*((float *)data));
            
            break;
            
        case CONTROLBUS_ASSERT_RESET:
            if (!resetCount)
                postNotification(this, CONTROLBUS_RESET_DID_ASSERT, NULL);
            
            resetCount++;
            
            return true;
            
        case CONTROLBUS_CLEAR_RESET:
            resetCount--;
            
            if (!resetCount)
                postNotification(this, CONTROLBUS_RESET_DID_CLEAR, NULL);
            
            return true;
            
        case CONTROLBUS_ASSERT_IRQ:
            if (!irqCount)
                postNotification(this, CONTROLBUS_IRQ_DID_ASSERT, NULL);
            
            irqCount++;
            
            return true;
            
        case CONTROLBUS_CLEAR_IRQ:
            if (!irqCount)
                postNotification(this, CONTROLBUS_IRQ_DID_CLEAR, NULL);
            
            irqCount--;
            
            return true;
            
        case CONTROLBUS_ASSERT_NMI:
            if (!nmiCount)
                postNotification(this, CONTROLBUS_NMI_DID_ASSERT, NULL);
            
            nmiCount++;
            
            return true;
            
        case CONTROLBUS_CLEAR_NMI:
            if (!nmiCount)
                postNotification(this, CONTROLBUS_NMI_DID_CLEAR, NULL);
            
            nmiCount--;
            
            return true;
            
        case CONTROLBUS_IS_RESET_ASSERTED:
            *((bool *)data) = (resetCount != 0);
            
            return true;
            
        case CONTROLBUS_IS_IRQ_ASSERTED:
            *((bool *)data) = (irqCount != 0);
            
            return true;
            
        case CONTROLBUS_IS_NMI_ASSERTED:
            *((bool *)data) = (nmiCount != 0);
            
            return true;
    }
    
    return false;
}

void ControlBus::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == audio)
    {
        if (powerState != CONTROLBUS_POWERSTATE_ON)
            return;
        
        AudioBuffer *buffer = (AudioBuffer *)data;
        
        audioBufferStart = cycles;
        sampleToCycleRatio = buffer->sampleRate / clockFrequency;
        
        scheduleTimer(NULL, ceil(buffer->frameNum / sampleToCycleRatio) - getCycles());
        
        while (true)
        {
            inEvent = true;
            
            cpuCycles += ceil(events.front().cycles * cpuClockMultiplier - cpuCycles);
            setPendingCPUCycles(floor(cpuCycles + getPendingCPUCycles()));
            runCPU();
            
            inEvent = false;
            
            OEComponent *component = events.front().component;
            cycles += events.front().cycles;
            cpuCycles -= events.front().cycles * cpuClockMultiplier;
            events.front().cycles = 0;
            events.pop_front();
            
            if (component)
            {
                OELong pendingCycles = -getCycles();
                component->notify(this, CONTROLBUS_TIMER_DID_FIRE, &pendingCycles);
            }
            else
                break;
        };
    }
    else if (sender == device)
    {
        switch (*((DeviceEvent *)data))
        {
            case DEVICE_POWERDOWN:
                setPowerState(CONTROLBUS_POWERSTATE_OFF);
                
                break;
                
            case DEVICE_SLEEP:
                if (powerState != CONTROLBUS_POWERSTATE_OFF)
                    setPowerState(CONTROLBUS_POWERSTATE_PAUSE);
                
                break;
                
            case DEVICE_WAKEUP:
                setPowerState(CONTROLBUS_POWERSTATE_ON);
                
                break;
                
            case DEVICE_COLDRESTART:
                setPowerState(CONTROLBUS_POWERSTATE_OFF);
                setPowerState(CONTROLBUS_POWERSTATE_ON);
                
                break;
                
            case DEVICE_WARMRESTART:
                if (resetOnPowerOn)
                {
                    postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
                    postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
                }
                
                break;
                
            case DEVICE_DEBUGGERBREAK:
                postMessage(this, CONTROLBUS_ASSERT_NMI, NULL);
                postMessage(this, CONTROLBUS_CLEAR_NMI, NULL);
                
                break;
                
            default:
                break;
        }
    }
}

void ControlBus::setPowerState(ControlBusPowerState value)
{
    if (powerState == value)
        return;
    
    ControlBusPowerState lastPowerState = powerState;
    powerState = value;
    
    updatePowerState();
    
    postNotification(this, CONTROLBUS_POWERSTATE_DID_CHANGE, &powerState);
    
    if (resetOnPowerOn && (lastPowerState == CONTROLBUS_POWERSTATE_OFF))
    {
        postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
        postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
    }
}

void ControlBus::updatePowerState()
{
    string stateLabel;
    
    switch (powerState)
    {
        case CONTROLBUS_POWERSTATE_OFF:
            stateLabel = "Powered Off";
            
            break;
            
        case CONTROLBUS_POWERSTATE_HIBERNATE:
            stateLabel = "Hibernated";
            
            break;
            
        case CONTROLBUS_POWERSTATE_SLEEP:
            stateLabel = "Sleeping";
            
            break;
            
        case CONTROLBUS_POWERSTATE_STANDBY:
            stateLabel = "Stand By";
            
            break;
            
        case CONTROLBUS_POWERSTATE_PAUSE:
            stateLabel = "Paused";
            
            break;
            
        case CONTROLBUS_POWERSTATE_ON:
            stateLabel = "Powered On";
            
            break;
            
        default:
            stateLabel = "Unknown";
            
            break;
    }
    
    device->postMessage(this, DEVICE_SET_STATELABEL, &stateLabel);
    device->postMessage(this, DEVICE_UPDATE, NULL);
    
    setActivity(powerState == CONTROLBUS_POWERSTATE_ON);
}

void ControlBus::setActivity(bool value)
{
    if (activity == value)
        return;
    
    activity = value;
    
    updateActivity();
}

void ControlBus::updateActivity()
{
    device->postMessage(this, (activity ?
                               DEVICE_ASSERT_ACTIVITY : 
                               DEVICE_CLEAR_ACTIVITY), NULL);
}

inline OESLong ControlBus::getPendingCPUCycles()
{
    OESLong value;
    
    cpu->postMessage(this, CPU_GET_PENDINGCYCLES, &value);
    
    return value;
}

inline void ControlBus::setPendingCPUCycles(OESLong value)
{
    cpu->postMessage(this, CPU_SET_PENDINGCYCLES, &value);
}

inline void ControlBus::runCPU()
{
    cpu->postMessage(this, CPU_RUN, &cpuCycles);
}

OELong ControlBus::getCycles()
{
    return floor((cpuCycles - getPendingCPUCycles()) / cpuClockMultiplier);
}

void ControlBus::scheduleTimer(OEComponent *component, OELong cycles)
{
    cycles += getCycles();
    
    list<ControlBusEvent>::iterator i;
    for (i = events.begin();
         i != events.end();
         i++)
    {
        if (cycles < i->cycles)
        {
            if ((i == events.begin()) && inEvent)
            {
                OESLong doneCPUCycles = floor(cpuCycles - getPendingCPUCycles());
                cpuCycles -= floor(cpuCycles);
                
                cpuCycles += ceil(cycles * cpuClockMultiplier - cpuCycles);
                setPendingCPUCycles(cpuCycles - doneCPUCycles);
            }
            
            i->cycles -= cycles;
            
            break;
        }
        
        cycles -= i->cycles;
    }
    
    ControlBusEvent event;
    
    event.cycles = cycles;
    event.component = component;
    
    events.insert(i, event);
}

void ControlBus::invalidateTimers(OEComponent *component)
{
    OELong cycles = 0;
    
    list<ControlBusEvent>::iterator i;
    for (i = events.begin();
         i != events.end();
         i++)
    {
        if (i->component == component)
        {
            cycles = i->cycles;
            events.erase(i);
        }
        else if (cycles)
        {
            i->cycles += cycles;
            cycles = 0;
            
            if ((i == events.begin()) && inEvent)
            {
                OESLong doneCPUCycles = floor(cpuCycles - getPendingCPUCycles());
                cpuCycles -= floor(cpuCycles);
                
                cpuCycles += ceil(events.front().cycles * cpuClockMultiplier - cpuCycles);
                setPendingCPUCycles(cpuCycles - doneCPUCycles);
            }
        }
    }
}

void ControlBus::setCPUClockMultiplier(float value)
{
    float ratio = value / cpuClockMultiplier;
    
    OESLong pendingCPUCycles;
    
    pendingCPUCycles = getPendingCPUCycles();
    
    double doneCPUCycles = cpuCycles - pendingCPUCycles;
    
    pendingCPUCycles *= ratio;
    
    setPendingCPUCycles(pendingCPUCycles);
    
    cpuCycles = doneCPUCycles * ratio + pendingCPUCycles;
    
    cpuClockMultiplier = value;
}
