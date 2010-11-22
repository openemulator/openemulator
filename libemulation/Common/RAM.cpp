
/**
 * libemulation
 * RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls random access memory
 */

#include "RAM.h"

#include "ControlBusInterface.h"
#include "AddressDecoder.h"

RAM::RAM()
{
	controlBus = NULL;
	
	size = 0;
	mask = 0;
	data = NULL;
	datap = NULL;
}

RAM::~RAM()
{
	delete data;
}

bool RAM::setValue(string name, string value)
{
	if (name == "size")
		size = getInt(value);
	else if (name == "powerOnPattern")
		powerOnPattern = getCharVector(value);
	else
		return false;
	
	return true;
}

bool RAM::setRef(string name, OEComponent *ref)
{
	if (name == "controlBus")
	{
		replaceObserver(controlBus, ref, CONTROLBUS_POWERSTATE_DID_CHANGE);
		controlBus = ref;
	}
	else
		return false;
	
	return true;
}

bool RAM::setData(string name, OEData *data)
{
	if (name == "image")
		this->data = data;
	else
		return OEComponent::setData(name, data);
	
	return true;
}

bool RAM::getData(string name, OEData **data)
{
	if (name == "image")
	{
		int powerState;
		
		controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
		if (powerState <= CONTROLBUS_POWERSTATE_HIBERNATE)
			return false;
		
		*data = this->data;
	}
	else
		return false;
	
	return true;
}

bool RAM::init()
{
	if (size <= 0)
	{
		log("invalid RAM size");
		return false;
	}
	
	if (!data)
		data = new OEData();
	
	if (!powerOnPattern.size())
		powerOnPattern.resize(1);
	
	size = getNextPowerOf2(size);
	mask = size - 1;
	
	data->resize(size);
	datap = (OEUInt8 *) &data->front();
	
	return true;
}

bool RAM::postMessage(OEComponent *sender, int message, void *data)
{
	switch (message)
	{
		case RAM_GET_MEMORY:
			*((OEData **) data) = this->data;
			return true;
	}
	
	return false;
}

void RAM::notify(OEComponent *sender, int notification, void *data)
{
	switch (notification)
	{
		case CONTROLBUS_POWERSTATE_DID_CHANGE:
		{
			for (int i = 0; i < this->data->size(); i++)
				(*this->data)[i] = powerOnPattern[i % powerOnPattern.size()];
			break;
		}
	}
	
	return;
}

OEUInt8 RAM::read(OEAddress address)
{
	return datap[address & mask];
}

void RAM::write(OEAddress address, OEUInt8 value)
{
	datap[address & mask] = value;
}
