
/**
 * libemulator
 * Apple I System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple I system
 */

#include "math.h"

#include "AppleISystem.h"
#include "HostAudio.h"

AppleISystem::AppleISystem()
{
	isCPUExternal = false;
}

bool AppleISystem::setProperty(const string &name, const string &value)
{
	if (name == "isCPUExternal")
		isCPUExternal = getInt(value);
	else
		return false;
	
	return true;
}

bool AppleISystem::getProperty(const string &name, string &value)
{
	if (name == "isCPUExternal")
		value = getString(isCPUExternal);
	else
		return false;

	return true;
}

bool AppleISystem::connect(const string &name, OEComponent *component)
{
	if (name == "hostAudio")
		;
		// component->addObserver(this);
	else if (name == "cpu")
		cpu = component;
	else if (name == "cpuExternal")
		cpuSocket = component;
	else if (name == "cpuSocket")
		cpuSocket = component;
	else
		return false;
	
	return true;
}

void AppleISystem::notify(int notification, OEComponent *component, void *data)
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
		
	// postNotification APPLEISYSTEM_VBL
}

int AppleISystem::ioctl(int message, void *data)
{
	switch (message)
	{
		case APPLEISYSTEM_GET_SAMPLEINDEX:
		{
			float *index = (float *) data;
			*index = 0;
			
			break;
		}
	}
	
	return false;	
}
