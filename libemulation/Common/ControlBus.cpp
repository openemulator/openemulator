
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
	
	frequency = 1E6;
	frequencyDivider = 1.0;
	cpu = NULL;
	cpuSocket = NULL;
	resetOnPowerOn = false;
	
	powerState = CONTROLBUS_POWERSTATE_ON;
	resetCount = 0;
	irqCount = 0;
	nmiCount = 0;
	
	phase = 0;
}

bool ControlBus::setValue(string name, string value)
{
	if (name == "frequency")
		frequency = getInt(value);
	else if (name == "frequencyDivider")
		frequencyDivider = getInt(value);
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
				postMessage(this, CONTROLBUS_ASSERT_RESET, &value);
				value = false;
				postMessage(this, CONTROLBUS_CLEAR_RESET, &value);
			}
			return true;
		}
		case CONTROLBUS_GET_POWERSTATE:
			*((int *)data) = powerState;
			return true;
			
		case CONTROLBUS_ASSERT_RESET:
			resetCount++;
			if (resetCount == 1)
				OEComponent::notify(this, CONTROLBUS_RESET_DID_ASSERT, NULL);
			return true;
			
		case CONTROLBUS_CLEAR_RESET:
			resetCount--;
			if (resetCount == 0)
				OEComponent::notify(this, CONTROLBUS_RESET_DID_CLEAR, NULL);
			return true;
			
		case CONTROLBUS_ASSERT_IRQ:
			irqCount++;
			if (irqCount == 1)
				OEComponent::notify(this, CONTROLBUS_IRQ_DID_ASSERT, NULL);
			return true;
			
		case CONTROLBUS_CLEAR_IRQ:
			irqCount--;
			if (irqCount == 0)
				OEComponent::notify(this, CONTROLBUS_IRQ_DID_CLEAR, NULL);
			return true;
			
		case CONTROLBUS_ASSERT_NMI:
			nmiCount++;
			if (nmiCount == 1)
				OEComponent::notify(this, CONTROLBUS_NMI_DID_ASSERT, NULL);
			return true;
			
		case CONTROLBUS_CLEAR_NMI:
			nmiCount--;
			if (nmiCount == 0)
				OEComponent::notify(this, CONTROLBUS_NMI_DID_ASSERT, NULL);
			return true;
			
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

void ControlBus::notify(OEComponent *sender, int notification, void *data)
{
	HostAudioBuffer *buffer = (HostAudioBuffer *) data;
	float *out = buffer->output;
	
	for(int i = 0; i < buffer->frameNum; i++)
	{
		float x = 0.1 * sin(phase);
		phase += 2 * M_PI * 500.0 / buffer->sampleRate;
		
		for (int ch = 0; ch < buffer->channelNum; ch++)
			*out++ += x;
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
