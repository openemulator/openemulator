
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
	mmu = NULL;
	
	host = NULL;
	
	isPowered = 0;
	
	memory = NULL;
	datap = NULL;
	
	setSize(1);
	setMemory(new OEData());
	
	powerOnPattern.resize(1);
}

RAM::~RAM()
{
	delete memory;
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

bool RAM::setProperty(const string &name, const string &value)
{
	if (name == "size")
		setSize(getInt(value));
	else if (name == "powerOnPattern")
		powerOnPattern = getCharVector(value);
	else if (name == "mmuMap")
		mmuMap = value;
	else
		return false;
	 
	return true;
}

bool RAM::setData(const string &name, OEData *data)
{
	if (name == "image")
		setMemory(data);
	else
		return false;
	
	return true;
}

bool RAM::getData(const string &name, OEData **data)
{
	if (name == "image")
	{
		if (!isPowered)
			return false;
		
		*data = memory;
	}
	else
		return false;
	
	return true;
}

bool RAM::connect(const string &name, OEComponent *component)
{
	if (name == "mmu")
	{
		if (mmu)
			component->postEvent(NULL, ADDRESSDECODER_MAP, &mmuMap);
		mmu = component;
		if (mmu)
			component->postEvent(this, ADDRESSDECODER_MAP, &mmuMap);
	}
	else if (name == "host")
	{
		if (host)
			host->removeObserver(this, HOST_POWERSTATE_CHANGED);
		host = component;
		if (host)
			host->addObserver(this, HOST_POWERSTATE_CHANGED);
	}
	else
		return false;
	
	return true;
}

void RAM::notify(OEComponent *component, int notification, void *data)
{
	int powerState = *((int *) data);
	
	bool willBePowered = (powerState <= HOST_POWERSTATE_STANDBY);
	if (!isPowered && willBePowered)
	{
		for (int i = 0; i < memory->size(); i++)
			(*memory)[i] = powerOnPattern[i % powerOnPattern.size()];
		
		isPowered = willBePowered;
	}
}

OEUInt8 RAM::read(int address)
{
	return datap[address & mask];
}

void RAM::write(int address, OEUInt8 value)
{
	datap[address & mask] = value;
}
