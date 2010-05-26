
/**
 * libemulator
 * Apple II Memory Management Unit
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II D000-FFFF range.
 */

#include "AppleIIMMU.h"

#define APPLEIIMMU_ROM_OFFSET	0xd000
#define APPLEIIMMU_ROM_SIZE		0x3000

AppleIIMMU::AppleIIMMU()
{
	hostSystem = NULL;
	memoryMap = NULL;
	floatingBus = NULL;
	
	romD0 = NULL;
	romD8 = NULL;
	romE0 = NULL;
	romE8 = NULL;
	romF0 = NULL;
	romF8 = NULL;
}

AppleIIMMU::~AppleIIMMU()
{
//	if (hostSystem)
	//	hostSystem->removeObserver(this);
}

void AppleIIMMU::mapComponent(OEComponent *component)
{
	if (!component)
		return;
	
	OEMemoryMap componentMemoryMap;
	
	component->ioctl(OE_GET_MEMORYMAP, &componentMemoryMap);
	memoryMap->ioctl(OE_SET_MEMORYMAP, &componentMemoryMap);
}

void AppleIIMMU::mapFloatingBus()
{
	if (!floatingBus)
		return;
	
	OEMemoryMap componentMemoryMap;
	componentMemoryMap.component = floatingBus;
	componentMemoryMap.range = mappedRange;
	memoryMap->ioctl(OE_SET_MEMORYMAP, &componentMemoryMap);
}

void AppleIIMMU::updateRomEnable()
{
	if (romEnable)
	{
		mapComponent(romD0);
		mapComponent(romD8);
		mapComponent(romE0);
		mapComponent(romE8);
		mapComponent(romF0);
		mapComponent(romF8);
	}
	else
		mapFloatingBus();
}

void AppleIIMMU::setRomEnable(bool value)
{
	if (value == romEnable)
		return;
	
	romEnable = value;
	updateRomEnable();
}

int AppleIIMMU::ioctl(int message, void *data)
{
	switch(message)
	{
		case OE_CONNECT:
		{
			OEConnection *connection = (OEConnection *) data;
			if (connection->name == "hostSystem")
			{
				hostSystem = connection->component;
//				hostSystem->addObserver(this);
			}
			else if (connection->name == "floatingBus")
			{
				floatingBus = connection->component;
				romD0 = romD8 = romE0 = romE8 = romF0 = romF8 = floatingBus;
			}
			else if (connection->name == "memoryMap")
				memoryMap = connection->component;
			else if (connection->name == "romD0")
			{
				romD0 = connection->component;
				updateRomEnable();
			}
			else if (connection->name == "romD8")
			{
				romD8 = connection->component;
				updateRomEnable();
			}
			else if (connection->name == "romE0")
			{
				romE0 = connection->component;
				updateRomEnable();
			}
			else if (connection->name == "romE8")
			{
				romE8 = connection->component;
				updateRomEnable();
			}
			else if (connection->name == "romF0")
			{
				romF0 = connection->component;
				updateRomEnable();
			}
			else if (connection->name == "romF8")
			{
				romF8 = connection->component;
				updateRomEnable();
			}
			break;
		}
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "romEnable")
				setRomEnable(getInt(property->value));
			break;
		}
		case OE_GET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "romEnable")
				property->value = romEnable;
			else
				return false;
			
			return true;
		}
	}
	
	return false;
}

void AppleIIMMU::onNotification(OEComponent *component, int message, void *data)
{
/*	switch(message)
	{
		case HID_S_COLDRESTART:
		case HID_S_WARMRESTART:
			setRomEnable(true);
			break;
	}*/
}
