
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
#include "Host.h"

ControlBus::ControlBus()
{
	crystal = 1E6;
	frequencyDivider = 1.0;
	updateFrequency();
	
	resetOnPowerOn = false;
	irqCount = 0;
	
	host = NULL;
	master = NULL;
}

void ControlBus::updateFrequency()
{
	frequency = crystal / frequencyDivider;
}

bool ControlBus::setValue(string name, string value)
{
	if (name == "crystal")
	{
		crystal = getInt(value);
		updateFrequency();
	}
	else if (name == "frequencyDivider")
	{
		frequencyDivider = getInt(value);
		updateFrequency();
	}
	else if (name == "resetOnPowerOn")
		resetOnPowerOn = getInt(value);
	
	return true;
}

bool ControlBus::setComponent(string name, OEComponent *component)
{
	if (name == "host")
	{
		if (host)
		{
			host->removeObserver(this, HOST_POWERED_ON);
			host->removeObserver(this, HOST_AUDIO_FRAME_WILL_RENDER);
			host->removeObserver(this, HOST_HID_SYSTEM_CHANGED);
		}
		host = component;
		if (host)
		{
			host->addObserver(this, HOST_POWERED_ON );
			host->addObserver(this, HOST_AUDIO_FRAME_WILL_RENDER);
			host->addObserver(this, HOST_HID_SYSTEM_CHANGED);
		}
	}
	else if (name == "master")
		master = component;
	else if (name == "masterSocket")
		masterSocket = component;
	
	return true;
}

void ControlBus::notify(OEComponent *component, int notification, void *data)
{
	switch (notification)
	{
		case HOST_POWERED_ON:
			if (resetOnPowerOn)
			{
				bool value = true;
				postEvent(this, CONTROLBUS_SET_RESET, &value);
			}
			break;
			
		case HOST_AUDIO_FRAME_WILL_RENDER:
		{
			HostAudioBuffer *buffer = (HostAudioBuffer *) data;
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
			
		case HOST_HID_SYSTEM_CHANGED:
		{
			HostHIDEvent *event = (HostHIDEvent *) data;
			int value;
			
			switch (event->usageId)
			{
				case HOST_HID_S_POWERDOWN:
					value = HOST_POWERSTATE_OFF;
					host->postEvent(this, HOST_SET_POWERSTATE, &value);
					break;
					
				case HOST_HID_S_SLEEP:
					value = HOST_POWERSTATE_PAUSE;
					host->postEvent(this, HOST_SET_POWERSTATE, &value);
					break;
					
				case HOST_HID_S_WAKEUP:
					value = HOST_POWERSTATE_ON;
					host->postEvent(this, HOST_SET_POWERSTATE, &value);
					break;
					
				case HOST_HID_S_DEBUGGERBREAK:
				{
					bool value = true;
					postEvent(this, CONTROLBUS_SET_NMI, &value);
					
					value = false;
					postEvent(this, CONTROLBUS_SET_NMI, &value);
					break;
				}
			}
			break;
		}
	}
}

bool ControlBus::postEvent(OEComponent *component, int event, void *data)
{
	switch (event)
	{
		case CONTROLBUS_SET_RESET:
		{
			bool value = *((bool *) data);
			resetCount += value ? 1 : -1;
			OEComponent::notify(this, CONTROLBUS_RESET_CHANGED, NULL);
			return true;
		}
		case CONTROLBUS_SET_IRQ:
			if (!irqCount)
				OEComponent::notify(this, CONTROLBUS_IRQ_CHANGED, NULL);
			irqCount++;
			return true;
			
		case CONTROLBUS_SET_NMI:
			OEComponent::notify(this, CONTROLBUS_NMI_CHANGED, NULL);
			return true;
			
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
