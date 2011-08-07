
/**
 * libemulation
 * Control bus
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
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
	device = NULL;
	audio = NULL;
	cpu = NULL;
	cpuSocket = NULL;
	
	crystalFrequency = 1E6;
	cpuFrequencyDivider = 1.0;
	cpu = NULL;
	cpuSocket = NULL;
	resetOnPowerOn = false;
	
	powerState = CONTROLBUS_POWERSTATE_ON;
	resetCount = 0;
	irqCount = 0;
	nmiCount = 0;
}

bool ControlBus::setValue(string name, string value)
{
	if (name == "crystalFrequency")
		crystalFrequency = getInt(value);
	else if (name == "cpuFrequencyDivider")
		cpuFrequencyDivider = getInt(value);
	else if (name == "resetOnPowerOn")
		resetOnPowerOn = getInt(value);
    else if (name == "powerState")
        powerState = getInt(value);
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
			device->postMessage(this, DEVICE_CLEAR_ACTIVITY, NULL);
			device->removeObserver(this, DEVICE_EVENT_DID_OCCUR);
		}
		device = ref;
		if (device)
		{
			device->postMessage(this, DEVICE_ASSERT_ACTIVITY, NULL);
			device->addObserver(this, DEVICE_EVENT_DID_OCCUR);
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
	else if (name == "cpuSocket")
		cpuSocket = ref;
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
	
    postMessage(this, CONTROLBUS_SET_POWERSTATE, &powerState);
    
	return true;
}

void ControlBus::update()
{
	cpuFrequency = crystalFrequency / cpuFrequencyDivider;
}

bool ControlBus::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
	{
		case CONTROLBUS_SET_POWERSTATE:
            setPowerState(*((int *)data));
			return true;
            
		case CONTROLBUS_GET_POWERSTATE:
			*((int *)data) = powerState;
			return true;
			
		case CONTROLBUS_ASSERT_RESET:
			resetCount++;
			if (resetCount == 1)
				notify(this, CONTROLBUS_RESET_DID_ASSERT, NULL);
			return true;
			
		case CONTROLBUS_CLEAR_RESET:
			resetCount--;
			if (resetCount == 0)
				notify(this, CONTROLBUS_RESET_DID_CLEAR, NULL);
			return true;
			
		case CONTROLBUS_ASSERT_IRQ:
			irqCount++;
			if (irqCount == 1)
				notify(this, CONTROLBUS_IRQ_DID_ASSERT, NULL);
			return true;
			
		case CONTROLBUS_CLEAR_IRQ:
			irqCount--;
			if (irqCount == 0)
				notify(this, CONTROLBUS_IRQ_DID_CLEAR, NULL);
			return true;
			
		case CONTROLBUS_ASSERT_NMI:
			nmiCount++;
			if (nmiCount == 1)
				notify(this, CONTROLBUS_NMI_DID_ASSERT, NULL);
			return true;
			
		case CONTROLBUS_CLEAR_NMI:
			nmiCount--;
			if (nmiCount == 0)
				notify(this, CONTROLBUS_NMI_DID_ASSERT, NULL);
			return true;
			
		case CONTROLBUS_SCHEDULE_TIMER:
            // To-Do: timers
			return true;
			
		case CONTROLBUS_CLEAR_TIMERS:
            // To-Do: timers
			return true;
			
		case CONTROLBUS_GET_CLOCKCYCLE:
            // To-Do: timers
			return true;
			
		case CONTROLBUS_GET_AUDIOBUFFERINDEX:
            // To-Do: timers
			return true;
            
        case CONTROLBUS_REQUEST_BUS:
            break;
            
        case CONTROLBUS_RELEASE_BUS:
            break;
	}
	
	return false;
}

void ControlBus::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == audio)
    {
        AudioBuffer *buffer = (AudioBuffer *)data;
         
/*        scheduleTimer(NULL, cpuFrequency * buffer->frameNum / buffer->sampleRate);
        while (1)
        {
            int clocks = events.front().clocks;
            if (cpu)
                cpu->postMessage(this, CPU_RUN, &clocks);
            OEComponent *component = events.front().component;
            events.pop_front();
            if (!component)
                break;
            component->notify(this, CONTROLBUS_TIMER_DID_FIRE, NULL);
        }*/
    }
    else if (sender == device)
    {
        switch (*((int *)data))
        {
            case DEVICE_POWERDOWN:
                setPowerState(powerState == CONTROLBUS_POWERSTATE_OFF ?
                              CONTROLBUS_POWERSTATE_ON :
                              CONTROLBUS_POWERSTATE_OFF);
                break;
                
            case DEVICE_SLEEP:
                if (powerState != CONTROLBUS_POWERSTATE_OFF)
                    setPowerState(CONTROLBUS_POWERSTATE_PAUSE);
                break;
                
            case DEVICE_WAKEUP:
                if (powerState != CONTROLBUS_POWERSTATE_PAUSE)
                    setPowerState(CONTROLBUS_POWERSTATE_ON);
                break;
                
            case DEVICE_COLDRESTART:
                setPowerState(CONTROLBUS_POWERSTATE_OFF);
                setPowerState(CONTROLBUS_POWERSTATE_ON);
                break;
                
            case DEVICE_WARMRESTART:
                postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
                postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
                break;
                
            case DEVICE_DEBUGGERBREAK:
                postMessage(this, CONTROLBUS_ASSERT_NMI, NULL);
                postMessage(this, CONTROLBUS_CLEAR_NMI, NULL);
                break;
        }
    }
}

void ControlBus::setPowerState(int powerState)
{
    if (resetOnPowerOn &&
        !isPoweredOn(powerState) &&
        isPoweredOn(this->powerState))
    {
        postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
        postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
    }
    this->powerState = powerState;
    
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
}

void ControlBus::scheduleTimer(OEComponent *component, int clocks)
{
    int currentClock;
    cpu->postMessage(this, CPU_GET_CLOCKCYCLES, &currentClock);
    
    // Mido el tiempo actual
    //    int nextClock = currentClock + clocks;
    
    // Sump, parto (C en dos), inserto
    // Si cae al principio, actualizo la CPU
}

bool ControlBus::isPoweredOn(int powerState)
{
	return (powerState >= CONTROLBUS_POWERSTATE_HIBERNATE);
}
