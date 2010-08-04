
/**
 * libemulation
 * Address offset
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address offset
 */

#include "AddressOffset.h"
#include "AddressDecoder.h"

AddressOffset::AddressOffset()
{
	offset = 0;
}

bool AddressOffset::setProperty(const string &name, const string &value)
{
	if (name == "offset")
		offset = getInt(value);
	else
		return false;
	
	return true;
}

bool AddressOffset::connect(const string &name, OEComponent *component)
{
if (name == "component")
		this->component = component;
	else
		return false;
	
	return true;
}

OEUInt8 AddressOffset::read(int address)
{
	return component->read(address + offset);
}

void AddressOffset::write(int address, OEUInt8 value)
{
	component->write(address + offset, value);
}
