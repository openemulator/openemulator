
/**
 * libemulator
 * IOU
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic input-output unit
 */

#include "IOU.h"

IOU::IOU()
{
	for (int i = 0; i < IOU_MAPSIZE; i++)
		readMap[i] = writeMap[i] = 0;
}

void IOU::setFloatingBus(OEComponent *component)
{
	for (int i = 0; i < IOU_MAPSIZE; i++)
	{
		if (!readMap[i])
			readMap[i] = component;
		if (!writeMap[i])
			writeMap[i] = component;
	}
}

void IOU::setRange(OEComponent *component, int start, int end)
{
	start &= IOU_MAPMASK;
	end &= IOU_MAPMASK;
	for (int i = start; i < end; i++)
	{
		readMap[i] = component;
		writeMap[i] = component;
	}
}

int IOU::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "floatingBus")
				setFloatingBus(connection->component);
			else
			{
				OEIoctlMemoryRange memoryRange;
				connection->component->ioctl(OEIOCTL_GET_MEMORYRANGE, &memoryRange);
				setRange(connection->component, memoryRange.offset, memoryRange.size);
			}
			break;
		}
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "offset")
				offset = intValue(property->value);
		}
		case OEIOCTL_GET_MEMORYRANGE:
		{
			OEIoctlMemoryRange *memoryRange = (OEIoctlMemoryRange *) data;
			memoryRange->offset = offset;
			memoryRange->size = IOU_MAPSIZE;
			break;
		}
	}
	
	return 0;
}

int IOU::read(int address)
{
	return readMap[address & IOU_MAPMASK]->read(address);
}

void IOU::write(int address, int value)
{
	readMap[address & IOU_MAPMASK]->write(address, value);
}
