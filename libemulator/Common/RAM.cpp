
/**
 * libemulator
 * Generic RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic RAM segment
 */

#include "RAM.h"

#include "HostSystem.h"

int RAM::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "hostSystem")
				connection->component->addObserver(this);
		}
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "offset")
				offset = intValue(property->value);
			else if (property->name == "size")
				memory.resize(intValue(property->value));
			break;
		}
		case OEIOCTL_SET_DATA:
		{
			OEIoctlData *setData = (OEIoctlData *) data;
			if (setData->name == "image")
			{
				memory = setData->data;
				mask = nextPowerOf2(memory.size()) - 1;
			}
			break;
		}
		case OEIOCTL_GET_DATA:
		{
			OEIoctlData *getData = (OEIoctlData *) data;
			if (getData->name == "image")
				getData->data = memory;
			else
				return false;
			
			return true;
		}
		case OEIOCTL_GET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->offset = offset;
			memoryMap->size = memory.size();
			break;
		}
		case OEIOCTL_NOTIFY:
		{
			OEIoctlNotification *notification = (OEIoctlNotification *) data;
			if (notification->message == HID_S_COLDRESTART);
			// Set up reset pattern
			break;
		}
	}
	
	return false;
}	

int RAM::read(int address)
{
	return memory[address & mask];
}

void RAM::write(int address, int value)
{
	memory[address & mask] = value;
}
