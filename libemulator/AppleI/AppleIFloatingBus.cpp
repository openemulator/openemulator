
/**
 * libemulator
 * Apple I Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Floating Bus
 */

#include "AppleIFloatingBus.h"

int AppleIFloatingBus::ioctl(int message, void * data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "system")
				system = connection->component;
			else if (connection->name == "memory")
				memory = connection->component;
			break;
		}
	}
	
	return false;
}

int AppleIFloatingBus::read(int address)
{
	address = 0;
	
	// To-Do: Ask system for the current time, calculate last refresh byte.
	
	return memory->read(address);
}
