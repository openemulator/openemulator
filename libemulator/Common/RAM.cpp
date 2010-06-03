
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
	resetNotification = 0;
	reset = NULL;
	
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
		size = getLowerPowerOf2(getInt(value));
		if (size < 1)
			size = 1;
		memory.resize(size);
		mask = size - 1;
	}
	else if (name == "resetPattern")
		resetPattern = getCharVector(value);
	else if (name == "resetNotification")
		resetNotification = getInt(value);
	else
		return false;
	
	return true;
}

bool RAM::setData(const string &name, OEData &data)
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
		data = memory;
	else
		return false;
	
	return true;
}

bool RAM::connect(const string &name, OEComponent *component)
{
	if (name == "reset")
	{
		if (reset)
			reset->removeObserver(this, resetNotification);
		reset = component;
		if (reset)
			reset->addObserver(this, resetNotification);
	}
	else
		return false;
	
	return true;
}

void RAM::notify(int notification, OEComponent *component, void *data)
{
	for (int i = 0; i < memory.size(); i++)
		memory[i] = resetPattern[i % resetPattern.size()];
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
