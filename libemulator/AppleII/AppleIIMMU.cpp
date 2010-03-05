
/**
 * libemulator
 * Apple II Memory Management Unit
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II D000-FFFF range.
 */

#include "AppleIIMMU.h"

#include "HostSystem.h"

AppleIIMMU::AppleIIMMU()
{
	hostSystem = NULL;
}

AppleIIMMU::~AppleIIMMU()
{
	if (hostSystem)
		hostSystem->removeObserver(this);
}

void AppleIIMMU::mapComponent(OEComponent *component)
{
	OEIoctlMemoryMap memoryMap;
	
	component->ioctl(OEIOCTL_GET_MEMORYMAP, &memoryMap);
	memory->ioctl(OEIOCTL_SET_MEMORYMAP, &memoryMap);
}

int AppleIIMMU::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "hostSystem")
			{
				hostSystem = connection->component;
				hostSystem->addObserver(this);
			}
			else if (connection->name == "floatingBus")
			{
				floatingBus = connection->component;
				romD0 = romD8 = romE0 = romE8 = romF0 = romF8 = floatingBus;
			}
			else if (connection->name == "memory")
				memory = connection->component;
			else if (connection->name == "romD0")
				romD0 = connection->component;
			else if (connection->name == "romD8")
				romD8 = connection->component;
			else if (connection->name == "romE0")
				romE0 = connection->component;
			else if (connection->name == "romE8")
				romE8 = connection->component;
			else if (connection->name == "romF0")
				romF0 = connection->component;
			else if (connection->name == "romF8")
				romF8 = connection->component;
			break;
		}
		case APPLEIIMMU_DISABLE_ROM:
		{
			OEIoctlMemoryMap memoryMap;
			memoryMap.component = floatingBus;
			memoryMap.offset = 0xd000;
			memoryMap.size = 0x3000;
			memory->ioctl(OEIOCTL_SET_MEMORYMAP, &memoryMap);
			break;
		}
		case APPLEIIMMU_ENABLE_ROM:
		{
			mapComponent(romD0);
			mapComponent(romD8);
			mapComponent(romE0);
			mapComponent(romE8);
			mapComponent(romF0);
			mapComponent(romF8);
			break;
		}
	}
	
	return 0;
}

void AppleIIMMU::onNotification(OEComponent *component, int message, void *data)
{
	switch(message)
	{
		case HID_S_COLDRESTART:
		case HID_S_WARMRESTART:
			ioctl(APPLEIIMMU_ENABLE_ROM, NULL);
			break;
	}
}
