
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

bool ControlBus::setProperty(const string &name, const string &value)
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

bool ControlBus::connect(const string &name, OEComponent *component)
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
	
	return true;
}

void ControlBus::notify(OEComponent *component, int notification, void *data)
{
	switch (notification)
	{
		case HOST_POWERED_ON:
			if (resetOnPowerOn)
				postEvent(this, CONTROLBUS_ASSERT_RESET, NULL);
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
					postEvent(this, CONTROLBUS_ASSERT_NMI, NULL);
					break;
			}
			break;
		}
	}
}

bool ControlBus::postEvent(OEComponent *component, int event, void *data)
{
	switch (event)
	{
		case CONTROLBUS_ASSERT_RESET:
			OEComponent::notify(this, CONTROLBUS_RESET_ASSERTED, NULL);
			return true;
			
		case CONTROLBUS_ASSERT_IRQ:
			if (!irqCount)
				OEComponent::notify(this, CONTROLBUS_IRQ_ASSERTED, NULL);
			irqCount++;
			return true;
			
		case CONTROLBUS_CLEAR_IRQ:
			if (irqCount > 0)
			{
				irqCount--;
				if (!irqCount)
					OEComponent::notify(this, CONTROLBUS_IRQ_CLEARED, NULL);
			}
			return true;
			
		case CONTROLBUS_ASSERT_NMI:
			OEComponent::notify(this, CONTROLBUS_NMI_ASSERTED, NULL);
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
