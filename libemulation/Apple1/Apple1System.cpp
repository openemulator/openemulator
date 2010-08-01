
/**
 * libemulation
 * Apple I System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple I system
 */

#include "math.h"

#include "Apple1System.h"
#include "Host.h"

Apple1System::Apple1System()
{
	isCPUExternal = false;
}

bool Apple1System::setProperty(const string &name, const string &value)
{
	if (name == "isCPUExternal")
		isCPUExternal = getInt(value);
	else
		return false;
	
	return true;
}

bool Apple1System::getProperty(const string &name, string &value)
{
	if (name == "isCPUExternal")
		value = getString(isCPUExternal);
	else
		return false;

	return true;
}

bool Apple1System::connect(const string &name, OEComponent *component)
{
	if (name == "host")
		;
		// component->addObserver(this);
	else if (name == "cpu")
		cpu = component;
	else if (name == "cpuExternal")
		cpuSocket = component;
	else if (name == "cpuSocket")
		cpuSocket = component;
	else if (name == "memoryMap")
		cpuSocket = component;
	else if (name == "floatingBus")
		cpuSocket = component;
	else if (name == "slot1")
		cpuSocket = component;
	else if (name == "slot2")
		cpuSocket = component;
	else
		return false;
	
	return true;
}

void Apple1System::notify(OEComponent *component, int notification, void *data)
{
//	if (notification == HOSTAUDIO_RENDER_DID_START)
	HostAudioBuffer *buffer = (HostAudioBuffer *) data;
	float *out = buffer->output;
	int sampleNum = buffer->channelNum * buffer->frameNum;
		
	for(int i = 0; i < sampleNum; i++)
	{
		*out++ += 0.05 * sin(phase);
		phase += 2 * M_PI * 220 / buffer->sampleRate;
	}
		
	// Implement simulation
		
	// postNotification APPLE1SYSTEM_VBL
}

int Apple1System::ioctl(int message, void *data)
{
	switch (message)
	{
		case APPLE1SYSTEM_GET_FRAMEINDEX:
		{
			float *index = (float *) data;
			*index = 0;
			
			break;
		}
	}
	
	return false;	
}
