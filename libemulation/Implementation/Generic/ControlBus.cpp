
/**
 * libemulation
 * Control bus
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a control bus with clock control and reset/IRQ/NMI lines
 */

// Notes:
// Timing diagram (for cpuClockMultiplier = 5/3)
//
// CPU cycle:         |  |  |  |  |  |  |  |  |  |  |  |
// Master cycle:        |    |    |    |    |    |    |
//
// Suppose we need to render 6 master cycles, then:
//
// blockSize:         |                               |
// blockCPUCycles:    |                             |
// blockOffset:         |                             |

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
    
    cycleCount = 0;
    inBlock = false;
    blockOffset = 0;
    
    cycleStart = 0;
    sampleToCycleRatio = 0;
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
            powerState = (ControlBusPowerState) getInt(value.substr(1));
    }
    else if (name == "resetOnPowerOn")
        resetOnPowerOn = getInt(value);
    else if (name == "resetCount")
        resetCount = (OEUInt32) getUInt(value);
    else if (name == "irqCount")
        irqCount = (OEUInt32) getUInt(value);
    else if (name == "nmiCount")
        nmiCount = (OEUInt32) getUInt(value);
    else
        return false;
    
    return true;
}

bool ControlBus::getValue(string name, string &value)
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
        {
            if (powerState == CONTROLBUS_POWERSTATE_ON)
                device->postMessage(this, DEVICE_CLEAR_ACTIVITY, NULL);
            
            device->removeObserver(this, DEVICE_DID_CHANGE);
        }
        device = ref;
        if (device)
        {
            if (powerState == CONTROLBUS_POWERSTATE_ON)
                device->postMessage(this, DEVICE_ASSERT_ACTIVITY, NULL);
            
            device->addObserver(this, DEVICE_DID_CHANGE);
        }
    }
    else if (name == "audio")
    {
        if (audio)
            audio->removeObserver(this, AUDIO_FRAME_IS_RENDERING);
        audio = ref;
        if (audio)
            audio->addObserver(this, AUDIO_FRAME_IS_RENDERING);
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
    
    setPowerState(powerState);
    
    if (powerState == CONTROLBUS_POWERSTATE_ON)
        device->postMessage(this, DEVICE_ASSERT_ACTIVITY, NULL);
    
    return true;
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
            
        case CONTROLBUS_ASSERT_RESET:
            resetCount++;
            
            if (resetCount == 1)
                postNotification(this, CONTROLBUS_RESET_DID_ASSERT, NULL);
            
            return true;
            
        case CONTROLBUS_CLEAR_RESET:
            resetCount--;
            
            if (resetCount == 0)
                postNotification(this, CONTROLBUS_RESET_DID_CLEAR, NULL);
            
            return true;
            
        case CONTROLBUS_ASSERT_IRQ:
            irqCount++;
            
            if (irqCount == 1)
                postNotification(this, CONTROLBUS_IRQ_DID_ASSERT, NULL);
            
            return true;
            
        case CONTROLBUS_CLEAR_IRQ:
            irqCount--;
            
            if (irqCount == 0)
                postNotification(this, CONTROLBUS_IRQ_DID_CLEAR, NULL);
            
            return true;
            
        case CONTROLBUS_ASSERT_NMI:
            nmiCount++;
            
            if (nmiCount == 1)
                postNotification(this, CONTROLBUS_NMI_DID_ASSERT, NULL);
            
            return true;
            
        case CONTROLBUS_CLEAR_NMI:
            nmiCount--;
            
            if (nmiCount == 0)
                postNotification(this, CONTROLBUS_NMI_DID_CLEAR, NULL);
            
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
            
        case CONTROLBUS_SET_CLOCKFREQUENCY:
            clockFrequency = *((float *)data);
            
            return true;
            
        case CONTROLBUS_GET_CLOCKFREQUENCY:
            *((float *)data) = clockFrequency;
            
            return true;
            
        case CONTROLBUS_GET_CYCLECOUNT:
            *((OEUInt64 *)data) = getCycleCount();
            
            return true;
            
        case CONTROLBUS_GET_AUDIOBUFFERINDEX:
            *((float *)data) = (getCycleCount() - cycleStart) * sampleToCycleRatio;
            
            return true;
            
        case CONTROLBUS_SCHEDULE_TIMER:
            scheduleTimer(sender, *((OEUInt64 *)data));
            
            return true;
            
        case CONTROLBUS_CLEAR_TIMERS:
            clearTimers(sender);
            
            return true;
            
        case CONTROLBUS_SET_CPUCLOCKMULTIPLIER:
            setCPUClockMultiplier(*((float *)data));
            
            break;
    }
    
    return false;
}

void ControlBus::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == audio)
    {
        AudioBuffer *buffer = (AudioBuffer *)data;
        
        cycleStart = cycleCount;
        sampleToCycleRatio = buffer->sampleRate / clockFrequency;
        
        scheduleTimer(NULL, buffer->frameNum / sampleToCycleRatio);
        
        OEComponent *component;
        
        inBlock = true;
        
        do
        {
            OEInt64 cpuCycles = floor(events.front().cycles * cpuClockMultiplier - blockOffset);
            blockOffset += cpuCycles;
            
            cpu->postMessage(this, CPU_RUN_CYCLES, &cpuCycles);
            
            component = events.front().component;
            if (component)
                component->notify(this, CONTROLBUS_TIMER_DID_FIRE, &cpuCycles);
            
            cycleCount += events.front().cycles;
            blockOffset -= events.front().cycles * cpuClockMultiplier - cpuCycles;
            
            events.pop_front();
        } while (component);
        
        inBlock = false;
    }
    else if (sender == device)
    {
        switch (*((ControlBusPowerState *)data))
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
    ControlBusPowerState lastPowerState = powerState;
    powerState = value;
    
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
    
    if ((lastPowerState == CONTROLBUS_POWERSTATE_ON) &&
        (powerState != CONTROLBUS_POWERSTATE_ON))
        device->postMessage(this, DEVICE_CLEAR_ACTIVITY, NULL);
    else if ((lastPowerState != CONTROLBUS_POWERSTATE_ON) &&
             (powerState == CONTROLBUS_POWERSTATE_ON))
        device->postMessage(this, DEVICE_ASSERT_ACTIVITY, NULL);
    
    if (lastPowerState != powerState)
        postNotification(this, CONTROLBUS_POWERSTATE_DID_CHANGE, &powerState);
    
    if (resetOnPowerOn &&
        (lastPowerState == CONTROLBUS_POWERSTATE_OFF) &&
        (powerState == CONTROLBUS_POWERSTATE_ON))
    {
        postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
        postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
    }
}

OEUInt64 ControlBus::getCycleCount()
{
    OEInt64 cpuCycles;
    
    cpu->postMessage(this, CPU_GET_CYCLES, &cpuCycles);
    
    return cycleCount + (OEUInt64) ((blockOffset - cpuCycles) / cpuClockMultiplier);
}

void ControlBus::scheduleTimer(OEComponent *component, OEUInt64 cycles)
{
    OEInt64 cpuCycles;
    
    cpu->postMessage(this, CPU_GET_CYCLES, &cpuCycles);
    
    cycles += (blockOffset - cpuCycles) / cpuClockMultiplier;
    
    list<ControlBusEvent>::iterator i;
    for (i = events.begin();
         i != events.end();
         i++)
    {
        if (cycles < i->cycles)
        {
            i->cycles -= cycles;
            
            if (inBlock && (i == events.begin()))
            {
                cpuCycles = floor(cycles * cpuClockMultiplier - blockOffset);
                blockOffset += cpuCycles;
                
                cpu->postMessage(this, CPU_SET_CYCLES, &cpuCycles);
            }
            
            break;
        }
        
        cycles -= i->cycles;
    }
    
    ControlBusEvent event;
    
    event.cycles = cycles;
    event.component = component;
    
    events.insert(i, event);
}

void ControlBus::clearTimers(OEComponent *component)
{
    OEUInt64 cycles = 0;
    
/*    list<ControlBusEvent>::iterator i;
    for (i = events.begin();
         i != events.end();
         i++)
    {
        if (i->component == component)
        {
            cycles = i->cycles;
            i = events.erase(i);
        }
        else if (cycles)
        {
            i->cycles += cycles;
            cycles = 0;
            
            if (inBlock && (i == events.begin()))
            {
                OEInt64 cpuCycles;
                
                cpu->postMessage(this, CPU_GET_CYCLES, &cpuCycles);
                
                cpuCycles = floor(cycles * cpuClockMultiplier - blockOffset + cpuCycles);
                blockOffset += cpuCycles;
                
                cpu->postMessage(this, CPU_SET_CYCLES, &cpuCycles);
            }
        }
    }*/
}

void ControlBus::setCPUClockMultiplier(float value)
{
    if (!inBlock)
        return;
    
    OEInt64 cpuCycles;
    
/*    cpu->postMessage(this, CPU_GET_CYCLES, &cpuCycles);
    
    cpuCycles = floor(cycles * value + cpuCycles - blockOffset);
    blockOffset += cpuCycles;
    
    cpu->postMessage(this, CPU_SET_CYCLES, &cpuCycles);

    
    
    
    
    float currentCycles = ((blockOffset - cpuCycles) * value /
                           cpuClockMultiplier);
    blockOffset = currentCycles - ceil(currentCycles);
    
    blockSize = events.front().cycles * value - blockOffset;
    blockCycles = floor(blockSize);
    blockOffset += blockCycles;
    
    cpuCycles = blockCycles - ceil(currentCycles);
    cpu->postMessage(this, CPU_SET_CYCLES, &cpuCycles);
    
    cpuClockMultiplier = value;*/
}
