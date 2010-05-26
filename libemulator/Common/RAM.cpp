
/**
 * libemulator
 * Generic RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic RAM segment
 */

#include "RAM.h"

RAM::RAM()
{
	size = 1;
	mask = 0;
	
	memory.resize(size);
	resetPattern.resize(size);
	resetPattern[0] = 0;
}

int RAM::ioctl(int message, void *data)
{
	switch(message)
	{
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "map")
				mappedRange.push_back(property->value);
			else if (property->name == "size")
			{
				size = getPreviousPowerOf2(getInt(property->value));
				if (size < 1)
					size = 1;
				memory.resize(size);
				mask = size - 1;
			}
			else if (property->name == "resetPattern")
				resetPattern = getCharVector(property->value);
			
			break;
		}
		case OE_SET_DATA:
		{
			OEData *setData = (OEData *) data;
			if (setData->name == "image")
			{
				memory = setData->data;
				memory.resize(size);
			}
			
			break;
		}
		case OE_GET_DATA:
		{
			OEData *getData = (OEData *) data;
			if (getData->name == "image")
				getData->data = memory;
			else
				return false;
			
			return true;
		}
		case OE_CONNECT:
		{
			OEConnection *connection = (OEConnection *) data;
//			if (connection->name == "hostSystem")
//				connection->component->addObserver(this);
			
			break;
		}
		case OE_GET_MEMORYMAP:
		{
			OEMemoryMap *memoryMap = (OEMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->range = mappedRange;
			
			break;
		}
		case OE_NOTIFY:
		{
/*			OENotification *notification = (OENotification *) data;
			if (notification->message == HOSTSYSTEM_RESET)
			{
				for (int i = 0; i < memory.size(); i++)
					memory[i] = resetPattern[i % resetPattern.size()];
			}
*/			
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
