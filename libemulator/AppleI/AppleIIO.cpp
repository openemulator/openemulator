
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
		case OE_CONNECT:
		{
			OEConnection *connection = (OEConnection *) data;
			if (connection->name == "pia")
				pia = connection->component;
			else if (connection->name == "floatingBus")
				floatingBus = connection->component;
			
			break;
		}
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "map")
				mappedRange.push_back(property->value);
			
			break;
		}
		case OE_GET_MEMORYMAP:
		{
			OEMemoryMap *memoryMap = (OEMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->range = mappedRange;
			
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
