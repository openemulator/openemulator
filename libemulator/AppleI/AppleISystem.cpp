
/**
 * libemulator
 * Apple I System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple I system
 */

#include "AppleISystem.h"
#include "HostAudio.h"

int AppleISystem::ioctl(int message, void *data)
{
	switch (message)
	{
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "internalCPUEnabled")
				internalCPUEnabled = getInt(property->value);
			
			break;
		}
		case OEIOCTL_GET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "internalCPUEnabled")
				property->value = internalCPUEnabled;
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
			if (notification->message == HOSTAUDIO_BUFFERRENDER)
			{
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
