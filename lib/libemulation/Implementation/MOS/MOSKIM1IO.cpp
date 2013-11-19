
/**
 * libemulation
 * MOS KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements MOS KIM-1 input/output
 */

#include "MOSKIM1IO.h"

#include "DeviceInterface.h"
#include "CanvasInterface.h"
#include "RS232Interface.h"

MOSKIM1IO::MOSKIM1IO()
{
	device = NULL;
	serialPort = NULL;
	audioOut = NULL;
	audioIn = NULL;
	
	canvas = NULL;
}

bool MOSKIM1IO::setValue(string name, string value)
{
	if (name == "viewPath")
		viewPath = value;
	else
		return false;
	
	return true;
}

bool MOSKIM1IO::setRef(string name, OEComponent *ref)
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
								   DEVICE_CONSTRUCT_DISPLAYCANVAS,
								   &canvas);
	}
	else if (name == "serialPort")
		serialPort = ref;
	else if (name == "audioOut")
		audioOut = ref;
	else if (name == "audioIn")
		audioIn = ref;
	else
		return false;
	
	return true;
}

bool MOSKIM1IO::init()
{
	if (!device)
	{
		logMessage("property 'device' undefined");
		return false;
	}
	
	if (!canvas)
	{
		logMessage("canvas could not be created");
		return false;
	}
	
//	CanvasBezel bezel = CANVAS_BEZEL_POWER;
	CanvasDisplayConfiguration configuration;
	OEImage image;
	image.load(viewPath);
	configuration.displayResolution = image.getSize();
	configuration.displayPixelDensity = 108;
	
//	canvas->postMessage(this, CANVAS_SET_BEZEL, &bezel);
	canvas->postMessage(this, CANVAS_CONFIGURE_DISPLAY, &configuration);
	canvas->postMessage(this, CANVAS_POST_IMAGE, &image);
	
	return true;
}

void MOSKIM1IO::notify(OEComponent *sender, int notification, void *data)
{
}
