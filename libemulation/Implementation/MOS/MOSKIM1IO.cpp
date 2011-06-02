
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
								   DEVICE_CREATE_CANVAS,
								   &canvas);
	}
	else if (name == "serialPort")
	{
		removeObserver(serialPort, RS232_DID_RECEIVE_DATA);
		serialPort = ref;
		addObserver(serialPort, RS232_DID_RECEIVE_DATA);
	}
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
		printLog("property 'device' undefined");
		return false;
	}
	
	if (!canvas)
	{
		printLog("canvas could not be created");
		return false;
	}
	
//	CanvasBezel bezel = CANVAS_BEZEL_POWER;
	CanvasDisplayConfiguration configuration;
	OEImage image;
	image.readFile(viewPath);
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
