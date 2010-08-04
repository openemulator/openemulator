
/**
 * libemulation
 * ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls read only memory
 */

#include "ROM.h"

#include "AddressDecoder.h"

ROM::ROM()
{
	memory = NULL;
	datap = NULL;
	
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

bool ROM::setResource(const string &name, OEData *data)
{
	if (name == "image")
		setMemory(data);
	else
		return false;
	
	return true;
}

int ROM::read(int address)
{
	return datap[address & mask];
}
