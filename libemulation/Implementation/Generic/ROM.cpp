
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
	data = NULL;
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
		return OEComponent::setData(name, data);
	
	return true;
}

bool ROM::init()
{
	if (!data)
	{
		logMessage("missing ROM");
		return false;
	}
	
	int size = getNextPowerOf2(data->size());
	data->resize(size);
	datap = (OEUInt8 *) &data->front();
	mask = size - 1;
	
	return true;
}

OEUInt8 ROM::read(OEAddress address)
{
	return datap[address & mask];
}
