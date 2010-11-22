
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
#include "HostAudioInterface.h"

ControlBus::ControlBus()
{
	hostAudio = NULL;
	
	crystal = 1E6;
	frequencyDivider = 1.0;
	master = NULL;
	masterSocket = NULL;
	resetOnPowerOn = false;
	
	powerState = CONTROLBUS_POWERSTATE_ON;
	resetCount = 0;
	irqCount = 0;
	nmiCount = 0;
	
	phase = 0;
}

bool ControlBus::setValue(string name, string value)
{
	if (name == "crystal")
		crystal = getInt(value);
	else if (name == "frequencyDivider")
		updateFrequency();
	else if (name == "resetOnPowerOn")
		resetOnPowerOn = getInt(value);
	else
		return false;
	
	return true;
}

bool ControlBus::setRef(string name, OEComponent *ref)
{
	if (name == "hostAudio")
	{
		replaceObserver(hostAudio, ref, HOST_AUDIO_FRAME_DID_BEGIN_RENDER);
		hostAudio = ref;
	}
	else if (name == "master")
		master = ref;
	else if (name == "masterSocket")
		masterSocket = ref;
	else
		return false;
	
	return true;
}

bool ControlBus::init()
{
	if (!hostAudio)
	{
		log("hostAudio undefined");
		return false;
	}
	
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
				OEComponent::notify(this, CONTROLBUS_RESET_DID_CHANGE, &resetCount);
			return true;
		}
		case CONTROLBUS_SET_IRQ:
		{
			int oldIRQCount = irqCount;
			irqCount += *((bool *) data) ? 1 : -1;
			if (!oldIRQCount != !irqCount)
				OEComponent::notify(this, CONTROLBUS_IRQ_DID_CHANGE, &irqCount);
			return true;
		}
		case CONTROLBUS_SET_NMI:
		{
			int oldNMICount = nmiCount;
			nmiCount += *((bool *) data) ? 1 : -1;
			if (!oldNMICount != !nmiCount)
				OEComponent::notify(this, CONTROLBUS_NMI_DID_CHANGE, &nmiCount);
			return true;
		}
		case CONTROLBUS_SCHEDULE_TIMER:
			return true;
			
		case CONTROLBUS_INVALIDATE_TIMER:
			return true;
			
		case CONTROLBUS_GET_CLOCKCYCLE:
			return true;
			
		case CONTROLBUS_GET_AUDIOBUFFERINDEX:
			return true;
	}
	
	return false;
}

void ControlBus::updateFrequency()
{
	frequency = crystal / frequencyDivider;
}

bool ControlBus::isPoweredOn(int powerState)
{
	return (powerState >= CONTROLBUS_POWERSTATE_HIBERNATE);
}
