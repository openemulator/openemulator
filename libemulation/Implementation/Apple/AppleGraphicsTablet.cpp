
/**
 * libemulation
 * Apple Graphics Tablet
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Graphics Tablet.
 */

#include "AppleGraphicsTablet.h"

#include "DeviceInterface.h"
#include "CanvasInterface.h"

AppleGraphicsTablet::AppleGraphicsTablet()
{
	device = NULL;
	canvas = NULL;
}

bool AppleGraphicsTablet::setValue(string name, string value)
{
	if (name == "viewPath")
		viewPath = value;
	else
		return false;
	
	return true;
}

bool AppleGraphicsTablet::setRef(string name, OEComponent *ref)
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
	else
		return false;
	
	return true;
}

bool AppleGraphicsTablet::init()
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
	
	CanvasDisplayConfiguration configuration;
	OEImage image;
	image.readFile(viewPath);
	configuration.displayResolution = image.getSize();
	configuration.displayPixelDensity = OEMakeSize(50, 50);
	CanvasBezel bezel = CANVAS_BEZEL_POWER;
	
	canvas->postMessage(this, CANVAS_SET_BEZEL, &bezel);
	canvas->postMessage(this, CANVAS_CONFIGURE_DISPLAY, &configuration);
	canvas->postMessage(this, CANVAS_POST_IMAGE, &image);
	
	return true;
}
