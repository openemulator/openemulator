
/**
 * libemulation
 * Generic RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic RAM segment
 */

#include "RAM.h"

#include "Host.h"

RAM::RAM()
{
	host = NULL;
	
	powered = 0;
	
	memory = new OEData();
	updateMemory(1);
	
	resetPattern.resize(1);
	resetPattern[0] = 0;
}

RAM::~RAM()
{
	memory->release();
}

void RAM::updateMemory(int size)
{
	size = getNextPowerOf2(size);
	if (size < 1)
		size = 1;
	memory->resize(size);
	mask = size - 1;
	data = memory->getData();
}

bool RAM::setProperty(const string &name, const string &value)
{
	if (name == "map")
		mappedRange = value;
	else if (name == "size")
	{
		size = getInt(value);
		updateMemory(size);
	}
	else if (name == "resetPattern")
		resetPattern = getCharVector(value);
	else
		return false;
	
	return true;
}

bool RAM::setData(const string &name, OEData *data)
{
	if (name == "image")
	{
		memory->release();
		memory = data;
		memory->retain();
		
		updateMemory(size);
	}
	else
		return false;
	
	return true;
}

bool RAM::getData(const string &name, OEData **data)
{
	if (name == "image")
	{
		if (powered)
			*data = memory;
	}
	else
		return false;
	
	return true;
}

bool RAM::connect(const string &name, OEComponent *component)
{
	if (name == "host")
	{
		if (host)
		{
			host->removeObserver(this, HOST_POWERSTATE_DID_CHANGE);
			host->removeObserver(this, HOST_HID_SYSTEM_EVENT);
		}
		
		host = component;
		
		if (host)
		{
			host->addObserver(this, HOST_POWERSTATE_DID_CHANGE);
			host->addObserver(this, HOST_HID_SYSTEM_EVENT);
		}
	}
	else
		return false;
	
	return true;
}

void RAM::notify(int notification, OEComponent *component, void *data)
{
	switch (notification)
	{
		case HOST_POWERSTATE_DID_CHANGE:
		{
			int powerState = *((int *) data);
			
			powered = (powerState <= HOST_POWERSTATE_STANDBY);
		}
		case HOST_HID_SYSTEM_EVENT:
		{
			HostHIDEvent *event = (HostHIDEvent *)data;
			if (event->usageId == HOST_HID_S_COLDRESTART)
			{
				for (int i = 0; i < memory->size(); i++)
					memory[i] = resetPattern[i % resetPattern.size()];
			}
			break;
		}
	}
}

bool RAM::getMemoryMap(string &value)
{
	value = mappedRange;
	
	return true;
}

OEUInt8 RAM::read(int address)
{
	return data[address & mask];
}

void RAM::write(int address, OEUInt8 value)
{
	data[address & mask] = value;
}
