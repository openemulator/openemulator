
/**
 * libemulation
 * Apple I input/output
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple I input/output PIA memory
 */

#include "Apple1IO.h"

#include "CanvasInterface.h"
#include "MC6821.h"

#define APPLE1KEYBOARD_MASK	0x40

Apple1IO::Apple1IO()
{
	terminal = NULL;
}

bool Apple1IO::setRef(string name, OEComponent *ref)
{
	if (name == "terminal")
		terminal = ref;
	else
		return false;
	
	return true;
}

void Apple1IO::notify(OEComponent *sender, int notification, void *data)
{
    /*	HostHIDEvent *event = (HostHIDEvent *) data;
     
     key = event->usageId;
     
     bool value = true;
     pia->postEvent(pia, MC6821_SET_CA1, &value);
     */
}

OEUInt8 Apple1IO::read(OEAddress address)
{
	/*
     bool value = false;
     pia->postEvent(pia, MC6821_SET_CA1, &value);
     */
	return key | APPLE1KEYBOARD_MASK;
}
