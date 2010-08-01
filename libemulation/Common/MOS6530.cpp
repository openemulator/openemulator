
/**
 * libemulation
 * MOS6530
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6530 (ROM, RAM, I/O, Timer)
 */

#include "MOS6530.h"

#include "MemoryMap.h"

MOS6530::MOS6530()
{
	portA = NULL;
	ddrA = 0;
	dataA = 0;

	portB = NULL;
	ddrB = 0;
	dataB = 0;
	
	resetNotification = 0;
	reset = NULL;
	
	irqEvent = 0;
	irq = NULL;
}

bool MOS6530::setProperty(const string &name, const string &value)
{
	if (name == "ddrA")
		ddrA = getInt(value);
	else if (name == "dataA")
		dataA = getInt(value);
	else if (name == "ddrB")
		ddrB = getInt(value);
	else if (name == "dataB")
		dataB = getInt(value);
	else if (name == "resetNotification")
		resetNotification = getInt(value);
	else if (name == "irqEvent")
		irqEvent = getInt(value);
	else if (name == "mmuMap")
		mmuMap = getInt(value);
	else
		return false;
	
	return true;
}

bool MOS6530::getProperty(const string &name, string &value)
{
	if (name == "ddrA")
		value = getHex(ddrA);
	else if (name == "dataA")
		value = getHex(dataA);
	else if (name == "ddrB")
		value = getHex(ddrB);
	else if (name == "dataB")
		value = getHex(dataB);
	else
		return false;
	
	return true;
}

bool MOS6530::connect(const string &name, OEComponent *component)
{
	if (name == "portA")
		portA = component;
	else if (name == "portB")
		portB = component;
	else if (name == "reset")
		reset = component;
	else if (name == "irq")
	{
		if (reset)
			reset->removeObserver(this, resetNotification);
		
		reset = component;
		
		if (reset)
			reset->addObserver(this, resetNotification);
	}
	else if (name == "mmu")
		component->postEvent(this, MEMORYMAP_MAP, &mmuMap);
	else
		return false;
	
	return true;
}

OEUInt8 MOS6530::read(int address)
{

}

void MOS6530::write(int address, OEUInt8 value)
{

}
