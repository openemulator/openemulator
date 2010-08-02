
/**
 * libemulation
 * Generic ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic ROM segment
 */

#include "ROM.h"

#include "AddressDecoder.h"

ROM::ROM()
{
	memory = NULL;
	
	setMemory(new OEData());
}

ROM::~ROM()
{
	delete memory;
}

void ROM::setMemory(OEData *data)
{
	delete memory;
	memory = data;
	
	int size = getNextPowerOf2(memory->size());
	if (size < 1)
		size = 1;
	
	mask = size - 1;
	
	memory->resize(size);
	datap = &memory->front();
}

bool ROM::setProperty(const string &name, const string &value)
{
	if (name == "mmuMap")
		mmuMap = value;
	else
		return false;
	
	return true;
}

bool ROM::setResource(const string &name, OEData *data)
{
	if (name == "image")
		setMemory(data);
	else
		return false;
	
	return true;
}

bool ROM::connect(const string &name, OEComponent *component)
{
	if (name == "mmu")
	{
		if (mmu)
			component->postEvent(NULL, ADDRESSDECODER_MAP, &mmuMap);
		mmu = component;
		if (mmu)
			component->postEvent(this, ADDRESSDECODER_MAP, &mmuMap);
	}
	else
		return false;
	
	return true;
}

OEUInt8 ROM::read(int address)
{
	return datap[address & mask];
}
