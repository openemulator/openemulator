
/**
 * libemulation
 * Silentype
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Silentype.
 */

#include "AppleSilentype.h"

#include "DeviceInterface.h"
#include "CanvasInterface.h"

AppleSilentype::AppleSilentype()
{
	device = NULL;
	canvas = NULL;
}

bool AppleSilentype::setValue(string name, string value)
{
	if (name == "viewPath")
		viewPath = value;
	else
		return false;
	
	return true;
}

bool AppleSilentype::setRef(string name, OEComponent *ref)
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

bool AppleSilentype::init()
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
	
	OEImage image;
	image.readFile(viewPath);
	
	CanvasMode mode = CANVAS_MODE_PAPER;
	CanvasPaperConfiguration configuration;
	configuration.pageResolution = OEMakeSize(612, 864);
	configuration.pagePixelDensity = OEMakeSize(72, 72);
	
	canvas->postMessage(this, CANVAS_SET_MODE, &mode);
	canvas->postMessage(this, CANVAS_CONFIGURE_PAPER, &configuration);
	canvas->postMessage(this, CANVAS_PRINT_IMAGE, &image);
	
	return true;
}
