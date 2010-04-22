
/**
 * libemulator
 * Apple I IO
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Input Output range
 */

#include "AppleIIO.h"

#define APPLEIIO_MASK	0x10

int AppleIIO::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "pia")
				pia = connection->component;
			else if (connection->name == "floatingBus")
				floatingBus = connection->component;
			break;
		}
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "map")
				mapVector.push_back(property->value);
			break;
		}
		case OEIOCTL_GET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->mapVector = mapVector;
			break;
		}
	}
	
	return false;
}

int AppleIIO::read(int address)
{
	if (address & APPLEIIO_MASK)
		return pia->read(address);
	else
		return floatingBus->read(address);
}

void AppleIIO::write(int address, int value)
{
	if (address & APPLEIIO_MASK)
		pia->write(address, value);
	else
		floatingBus->write(address, value);
}
