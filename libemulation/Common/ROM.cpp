
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
	datap = (OEUInt8 *) &memory->front();
}

bool ROM::setData(string name, OEData *data)
{
	if (name == "image")
		setMemory(data);
	else
		return false;
	
	return true;
}

OEUInt8 ROM::read(OEAddress address)
{
	return datap[address & mask];
}
