
/**
 * libemulation
 * Control bus
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a control bus with clock control and reset/IRQ/NMI lines
 */

#include <math.h>

#include "Emulation.h"
#include "ControlBus.h"
#include "AudioInterface.h"

ControlBus::ControlBus()
{
	emulation = NULL;
	audio = NULL;
	
	crystalFrequency = 1E6;
	cpuFrequencyDivider = 1.0;
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
	if (name == "crystalFrequency")
		crystalFrequency = getInt(value);
	else if (name == "cpuFrequencyDivider")
		cpuFrequencyDivider = getInt(value);
	else if (name == "resetOnPowerOn")
		resetOnPowerOn = getInt(value);
	else
		return false;
	
	return true;
}

bool ControlBus::setRef(string name, OEComponent *ref)
{
	if (name == "emulation")
	{
		if (emulation)
			emulation->postMessage(this, EMULATION_SET_SYSTEM_EVENT_HANDLER, NULL);
		emulation = ref;
		if (emulation)
			emulation->postMessage(this, EMULATION_SET_SYSTEM_EVENT_HANDLER, this);
	}
	else if (name == "audio")
	{
		removeObserver(audio, AUDIO_FRAME_IS_RENDERING);
		audio = ref;
		addObserver(audio, AUDIO_FRAME_IS_RENDERING);
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
	if (!audio)
	{
		logMessage("ref to 'audio' undefined");
		return false;
	}
	
	if (!emulation)
	{
		logMessage("ref to 'emulation' undefined");
		return false;
	}
	
	string state = "Powered On";
	emulation->postMessage(this, EMULATION_SET_STATELABEL, &state);
		
	// Init running
	emulation->postMessage(this, EMULATION_ASSERT_RUNNING, NULL);
	
	updateCPUFrequency();
	
	return true;
}

bool ControlBus::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
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
			
			// To-Do: Change RUNNING state in emulation
			
			return true;
		}
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
/*	HostAudioBuffer *buffer = (HostAudioBuffer *) data;
	float *out = buffer->output;
	
	float freq = 440.0 + rand() * 2.0 / RAND_MAX;
	
	for(int i = 0; i < buffer->frameNum; i++)
	{
		float x = 0.01 * sin(phase);
		phase += 2 * M_PI * freq / buffer->sampleRate;
		
		for (int ch = 0; ch < buffer->channelNum; ch++)
			*out++ += x;
	}*/
	
	return OEComponent::notify(sender, notification, data);

}

void ControlBus::updateCPUFrequency()
{
	cpuFrequency = crystalFrequency / cpuFrequencyDivider;
}

bool ControlBus::isPoweredOn(int powerState)
{
	return (powerState >= CONTROLBUS_POWERSTATE_HIBERNATE);
}
