
/**
 * libemulation
 * RAM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls random access memory
 */

#include "RAM.h"
#include "ControlBus.h"
#include "AddressDecoder.h"

RAM::RAM()
{
	size = 0;
	data = NULL;
	
	controlBus = NULL;
	powerState = CONTROLBUS_POWERSTATE_ON;
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
		setObserver(controlBus, ref, CONTROLBUS_POWERSTATE_DID_CHANGE);
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
		if (controlBus && (powerState <= CONTROLBUS_POWERSTATE_HIBERNATE))
			*data = NULL;
		else
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
		logMessage("invalid RAM size");
		return false;
	}
	
	if (!data)
		data = new OEData();
	
	if (!powerOnPattern.size())
		powerOnPattern.resize(1);
	else
		powerOnPattern.resize(getNextPowerOf2(powerOnPattern.size()));
	
	if (controlBus)
		controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
	
	size = getNextPowerOf2(size);
	data->resize(size);
	datap = (OEUInt8 *) &data->front();
	mask = size - 1;
	
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
	
	return OEComponent::postMessage(sender, message, data);
}

void RAM::notify(OEComponent *sender, int notification, void *data)
{
	bool newPowerState = *((int *)data);
	if ((powerState <= CONTROLBUS_POWERSTATE_HIBERNATE) &&
		(newPowerState > CONTROLBUS_POWERSTATE_HIBERNATE))
	{
		int mask = powerOnPattern.size() - 1;
		for (int i = 0; i < this->data->size(); i++)
			datap[i] = powerOnPattern[i & mask];
	}
	powerState = newPowerState;
}

OEUInt8 RAM::read(OEAddress address)
{
	return datap[address & mask];
}

void RAM::write(OEAddress address, OEUInt8 value)
{
	datap[address & mask] = value;
}
