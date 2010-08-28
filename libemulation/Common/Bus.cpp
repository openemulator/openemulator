
/**
 * libemulation
 * Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a bus with clock control and reset/IRQ/NMI lines
 */

#include <math.h>

#include "Bus.h"
#include "Host.h"

Bus::Bus()
{
	floatingBus = 0;
	
	crystal = 1.0;
	divider = 1.0;
	updateFrequency();
	
	resetOnPowerOn = false;
	irqCount = 0;
	
	host = NULL;
	busMaster = NULL;
}

void Bus::updateFrequency()
{
	frequency = crystal / divider;
}

bool Bus::setProperty(const string &name, const string &value)
{
	if (name == "floatingBus")
		floatingBus = getInt(value);
	else if (name == "crystal")
	{
		crystal = getInt(value);
		updateFrequency();
	}
	else if (name == "frequencyDivider")
	{
		divider = getInt(value);
		updateFrequency();
	}
	else if (name == "resetOnPowerOn")
		resetOnPowerOn = getInt(value);
	else
		return AddressDecoder::setProperty(name, value);
	
	return true;
}

bool Bus::connect(const string &name, OEComponent *component)
{
	if (name == "host")
	{
		if (host)
		{
			host->removeObserver(this, HOST_POWERED_ON);
			host->removeObserver(this, HOST_AUDIO_RENDER_DID_BEGIN);
			host->removeObserver(this, HOST_HID_SYSTEM_CHANGED);
		}
		host = component;
		if (host)
		{
			host->addObserver(this, HOST_POWERED_ON );
			host->addObserver(this, HOST_AUDIO_RENDER_DID_BEGIN);
			host->addObserver(this, HOST_HID_SYSTEM_CHANGED);
		}
	}
	else if (name == "busMaster")
		busMaster = component;
	else
		return AddressDecoder::connect(name, component);
	
	return true;
}

void Bus::notify(OEComponent *component, int notification, void *data)
{
	switch (notification)
	{
		case HOST_POWERED_ON:
			if (resetOnPowerOn)
				postEvent(this, BUS_ASSERT_RESET, NULL);
			break;
			
		case HOST_AUDIO_RENDER_DID_BEGIN:
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
					postEvent(this, BUS_ASSERT_NMI, NULL);
					break;
			}
			break;
		}
	}
}

bool Bus::postEvent(OEComponent *component, int event, void *data)
{
	switch (event)
	{
		case BUS_ASSERT_RESET:
			OEComponent::notify(this, BUS_RESET_ASSERTED, NULL);
			return true;
			
		case BUS_ASSERT_IRQ:
			if (!irqCount)
				OEComponent::notify(this, BUS_IRQ_ASSERTED, NULL);
			irqCount++;
			return true;
			
		case BUS_CLEAR_IRQ:
			if (irqCount > 0)
			{
				irqCount--;
				if (!irqCount)
					OEComponent::notify(this, BUS_IRQ_CLEARED, NULL);
			}
			return true;
			
		case BUS_ASSERT_NMI:
			OEComponent::notify(this, BUS_NMI_ASSERTED, NULL);
			return true;
			
		case BUS_ADD_TIMER:
			return true;
			
		case BUS_REMOVE_TIMER:
			return true;
			
		case BUS_GET_CYCLE:
			return true;
			
		case BUS_GET_AUDIO_BUFFER_INDEX:
			return true;
	}
	
	return AddressDecoder::postEvent(component, event, data);
}
