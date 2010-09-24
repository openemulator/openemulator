
/**
 * libemulation
 * RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls random access memory
 */

#include "RAM.h"

#include "Host.h"
#include "AddressDecoder.h"

RAM::RAM()
{
	memory = NULL;
	datap = NULL;
	
	host = NULL;
	
	setSize(1);
	setMemory(new OEData());
	
	powerOnPattern.resize(1);
}

RAM::~RAM()
{
	delete memory;
}

bool RAM::setValue(string name, string value)
{
	if (name == "size")
		setSize(getInt(value));
	else if (name == "powerOnPattern")
		powerOnPattern = getCharVector(value);
	else
		return false;
	 
	return true;
}

bool RAM::setComponent(string name, OEComponent *component)
{
	if (name == "host")
	{
		if (host)
			host->removeObserver(this, HOST_POWERED_ON);
		host = component;
		if (host)
			host->addObserver(this, HOST_POWERED_ON);
	}
	else
		return false;
	
	return true;
}

bool RAM::setData(string name, OEData *data)
{
	if (name == "image")
		setMemory(data);
	else
		return false;
	
	return true;
}

bool RAM::getData(string name, OEData **data)
{
	if (name == "image")
	{
		int powerState;
		
		host->postEvent(this, HOST_GET_POWERSTATE, &powerState);
		if (powerState <= HOST_POWERSTATE_HIBERNATE)
			return false;
		
		*data = memory;
	}
	else
		return false;
	
	return true;
}

void RAM::notify(OEComponent *component, int notification, void *data)
{
	switch (notification)
	{
		case HOST_POWERED_ON:
			for (int i = 0; i < memory->size(); i++)
				(*memory)[i] = powerOnPattern[i % powerOnPattern.size()];
			break;
	}
	
	return;
}

bool RAM::postEvent(OEComponent *component, int event, void *data)
{
	switch (event)
	{
		case RAM_GET_MEMORY:
			*((OEData **) data) = (OEData *) memory;
			return true;
	}
	
	return false;
}

int RAM::read(int address)
{
	return datap[address & mask];
}

void RAM::write(int address, int value)
{
	datap[address & mask] = value;
}

void RAM::setSize(int value)
{
	value = getNextPowerOf2(value);
	if (value < 1)
		value = 1;
	
	size = value;
	mask = value - 1;
}

void RAM::setMemory(OEData *data)
{
	delete memory;
	memory = data;
	
	memory->resize(size);
	datap = &memory->front();
}


