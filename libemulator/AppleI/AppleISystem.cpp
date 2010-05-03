
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

int AppleISystem::ioctl(int message, void *data)
{
	switch (message)
	{
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "isCPUExternal")
				isCPUExternal = getInt(property->value);
			
			break;
		}
		case OEIOCTL_GET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "isCPUExternal")
				property->value = isCPUExternal;
			else
				return false;
			
			return true;
		}
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "hostAudio")
			{
				OEComponent *hostAudio = connection->component;
				hostAudio->addObserver(this);
			}
			else if (connection->name == "cpu")
				cpu = connection->component;
			else if (connection->name == "cpuSocket")
				cpuSocket = connection->component;
			
			break;
		}
		case OEIOCTL_NOTIFY:
		{
			OEIoctlNotification *notification = (OEIoctlNotification *) data;
			if (notification->message == HOSTAUDIO_RENDER_DID_START)
			{
				HostAudioBuffer *buffer = (HostAudioBuffer *) notification->data;
				float *out = buffer->output;
				int sampleNum = buffer->channelNum * buffer->frameNum;
				
				for(int i = 0; i < sampleNum; i++)
				{
					float value = 0.05 * sin(phase);
					*out++ = value;
					
					phase += 2 * M_PI * 220 / buffer->sampleRate;
				}
				
				// Implement simulation
				
				// postNotification APPLEISYSTEM_VBL
			}
			break;
		}
		case APPLEISYSTEM_GET_SAMPLEINDEX:
		{
			float *index = (float *) data;
			*index = 0;
			
			break;
		}
	}
	
	return false;	
}
