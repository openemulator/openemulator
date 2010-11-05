
/**
 * libemulation
 * Apple I input/output
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple I input/output
 */

#include "Apple1IO.h"

#include "HostInterface.h"
#include "MC6821.h"

#define APPLE1KEYBOARD_MASK	0x40

Apple1IO::Apple1IO()
{
	terminal = NULL;
}

bool Apple1IO::setComponent(string name, OEComponent *component)
{
	if (name == "terminal")
		terminal = component;
	else
		return false;
	
	return true;
}

void Apple1IO::notify(int notification, OEComponent *component, void *data)
{
/*	HostHIDEvent *event = (HostHIDEvent *) data;
	
	key = event->usageId;
	
	bool value = true;
	pia->postEvent(pia, MC6821_SET_CA1, &value);
 */
}

OEUInt8 Apple1IO::read(int address)
{
	/*
	bool value = false;
	pia->postEvent(pia, MC6821_SET_CA1, &value);
	*/
	return key | APPLE1KEYBOARD_MASK;
}
