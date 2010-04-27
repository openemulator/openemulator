
/**
 * libemulator
 * Host System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host system events
 */

#include "HostSystem.h"

HostSystem::HostSystem()
{
	runTime = 0;
	isPaused = false;
	
	isDMLUpdated = false;
}

int HostSystem::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "runTime")
				runTime = getFloat(property->value);
			else if (property->name == "isPaused")
				isPaused = getInt(property->value);
			
			break;
		}
		case OEIOCTL_GET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "runTime")
				property->value = runTime;
			else if (property->name == "isPaused")
				property->value = isPaused;
			
			break;
		}
		case OEIOCTL_POST_NOTIFICATION:
		{
			OEIoctlNotification *notification = (OEIoctlNotification *) data;
			postNotification(notification->message, notification->data);
			
			break;	
		}
		case HOSTSYSTEM_UPDATE_DML:
		{
			isDMLUpdated = *((bool *) data);
			break;
		}
		case HOSTSYSTEM_QUERY_DML_UPDATE:
			return isDMLUpdated;
		case HOSTSYSTEM_ADD_RUNTIME:
		{
			runTime += *((double *) data);
			break;
		}
		case HOSTSYSTEM_GET_RUNTIME:
		{
			*((double *) data) = runTime;
			break;
		}
		case HOSTSYSTEM_SET_PAUSE:
		{
			isPaused = *((bool *) data);
			break;
		}
		case HOSTSYSTEM_GET_PAUSE:
			return isPaused;
	}
	
	return false;
}
