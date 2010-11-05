
/**
 * libemulation
 * Control bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a control bus with clock control and reset/IRQ/NMI lines
 */

#include <math.h>

#include "ControlBus.h"
#include "HostInterface.h"

ControlBus::ControlBus()
{
	crystal = 1E6;
	frequencyDivider = 1.0;
	master = NULL;
	masterSocket = NULL;
	resetOnPowerOn = false;
	
	powerState = CONTROLBUS_POWERSTATE_ON;
	resetCount = 0;
	irqCount = 0;
	nmiCount = 0;
}

bool ControlBus::setValue(string name, string value)
{
	if (name == "crystal")
		crystal = getInt(value);
	else if (name == "frequencyDivider")
		updateFrequency();
	else if (name == "resetOnPowerOn")
		resetOnPowerOn = getInt(value);
	
	return true;
}

bool ControlBus::setRef(string name, OEComponent *ref)
{
	if (name == "master")
		master = ref;
	else if (name == "masterSocket")
		masterSocket = ref;
	
	return true;
}

bool ControlBus::init()
{
	updateFrequency();
	
	return true;
}

bool ControlBus::postMessage(OEComponent *component, int event, void *data)
{
	switch (event)
	{
		case CONTROLBUS_SET_POWERSTATE:
		{
			int oldPowerState = powerState;
			powerState = *((int *)data);
			if (!isPoweredOn(oldPowerState) &&
				isPoweredOn(powerState) &&
				resetOnPowerOn)
			{
				bool value = true;
				postMessage(this, CONTROLBUS_SET_RESET, &value);
				value = false;
				postMessage(this, CONTROLBUS_SET_RESET, &value);
			}
			return true;
		}
		case CONTROLBUS_GET_POWERSTATE:
		{
			*((int *)data) = powerState;
			return true;
		}
		case CONTROLBUS_SET_RESET:
		{
			int oldResetCount = resetCount;
			resetCount += *((bool *) data) ? 1 : -1;
			if (!oldResetCount != !resetCount)
				OEComponent::notify(this, CONTROLBUS_RESET_CHANGED, &resetCount);
			return true;
		}
		case CONTROLBUS_SET_IRQ:
		{
			int oldIRQCount = irqCount;
			irqCount += *((bool *) data) ? 1 : -1;
			if (!oldIRQCount != !irqCount)
				OEComponent::notify(this, CONTROLBUS_IRQ_CHANGED, &irqCount);
			return true;
		}			
		case CONTROLBUS_SET_NMI:
		{
			int oldNMICount = nmiCount;
			nmiCount += *((bool *) data) ? 1 : -1;
			if (!oldNMICount != !nmiCount)
				OEComponent::notify(this, CONTROLBUS_NMI_CHANGED, &nmiCount);
			return true;
		}			
		case CONTROLBUS_ADD_TIMER:
			return true;
			
		case CONTROLBUS_REMOVE_TIMER:
			return true;
			
		case CONTROLBUS_GET_CYCLE:
			return true;
			
		case CONTROLBUS_GET_AUDIO_BUFFER_INDEX:
			return true;
	}
	
	return false;
}

void ControlBus::notify(OEComponent *component, int notification, void *data)
{
	switch (notification)
	{
		case HOST_AUDIO_FRAME_WILL_RENDER:
		{
			HostAudioNotification *buffer = (HostAudioNotification *) data;
			float *out = buffer->output;
			int sampleNum = buffer->channelNum * buffer->frameNum;
			
			for(int i = 0; i < sampleNum; i++)
			{
				*out++ += 0.05 * sin(phase);
				phase += 2 * M_PI * 220 / buffer->sampleRate;
			}
			
			// Implement simulation
			break;
		}
	}
}

void ControlBus::updateFrequency()
{
	frequency = crystal / frequencyDivider;
}

bool ControlBus::isPoweredOn(int powerState)
{
	return (powerState >= CONTROLBUS_POWERSTATE_HIBERNATE);
}
