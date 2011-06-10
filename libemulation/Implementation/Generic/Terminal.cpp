
/**
 * libemulation
 * Terminal
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a terminal.
 */

#include "Terminal.h"

#include "DeviceInterface.h"

Terminal::Terminal()
{
	device = NULL;
	
	canvas = NULL;
}

bool Terminal::setRef(string name, OEComponent *ref)
{
	if (name == "device")
	{
		if (device)
			device->postMessage(this,
								DEVICE_DESTROY_CANVAS,
								&canvas);
		device = ref;
		if (device)
			device->postMessage(this,
								DEVICE_CONSTRUCT_CANVAS,
								&canvas);
	}
	else
		return false;
	
	return true;
}
