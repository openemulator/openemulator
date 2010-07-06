
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
	
	size = 1;
	mask = 0;
	
	memory.resize(size);
	resetPattern.resize(size);
	resetPattern[0] = 0;
}

bool RAM::setProperty(const string &name, const string &value)
{
	if (name == "map")
		mappedRange = value;
	else if (name == "size")
	{
		size = getNextPowerOf2(getInt(value));
		if (size < 1)
			size = 1;
		memory.resize(size);
		mask = size - 1;
	}
	else if (name == "resetPattern")
		resetPattern = getCharVector(value);
	else
		return false;
	
	return true;
}

bool RAM::setData(const string &name, const OEData &data)
{
	if (name == "image")
	{
		memory = data;
		memory.resize(size);
	}
	else
		return false;
	
	return true;
}

bool RAM::getData(const string &name, OEData &data)
{
	if (name == "image")
	{
		if (!powered)
			data = memory;
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
	if (notification == HOST_POWERSTATE_DID_CHANGE)
	{
		bool willBePowered = (*((int *) data) <= HOST_POWERSTATE_HIBERNATE);
		
		if (!powered && willBePowered)
		{
			for (int i = 0; i < memory.size(); i++)
				memory[i] = resetPattern[i % resetPattern.size()];
		}
		
		powered = willBePowered;
	}
}

bool RAM::getMemoryMap(string &value)
{
	value = mappedRange;
	
	return true;
}

OEUInt8 RAM::read(int address)
{
	return memory[address & mask];
}

void RAM::write(int address, OEUInt8 value)
{
	memory[address & mask] = value;
}
