
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
	size = 0;
	mask = 0;
	data = NULL;
	datap = NULL;
}

ROM::~ROM()
{
	delete data;
}

bool ROM::setData(string name, OEData *data)
{
	if (name == "image")
		this->data = data;
	else
		return false;
	
	return true;
}

bool ROM::init()
{
	if (!data)
	{
		log("missing ROM");
		return false;
	}
	
	size = getNextPowerOf2(size);
	mask = size - 1;
	
	data->resize(size);
	datap = (OEUInt8 *) &data->front();
	
	return true;
}

OEUInt8 ROM::read(OEAddress address)
{
	return datap[address & mask];
}
