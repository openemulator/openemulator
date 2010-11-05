
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

bool AddressOffset::setValue(string name, string value)
{
	if (name == "offset")
		offset = getInt(value);
	else
		return false;
	
	return true;
}

bool AddressOffset::setRef(string name, OEComponent *ref)
{
	if (name == "component")
		component = ref;
	else
		return false;
	
	return true;
}

bool AddressOffset::init()
{
	return !component;
}

OEUInt8 AddressOffset::read(OEAddress address)
{
	return component->read(address + offset);
}

void AddressOffset::write(OEAddress address, OEUInt8 value)
{
	component->write(address + offset, value);
}
