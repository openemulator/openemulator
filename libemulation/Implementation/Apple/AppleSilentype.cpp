
/**
 * libemulation
 * Apple Silentype
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Silentype printer
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
	if (name == "imagePath")
		imagePath = value;
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
								   DEVICE_CONSTRUCT_CANVAS,
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
		logMessage("property 'device' undefined");
		return false;
	}
	
	if (!canvas)
	{
		logMessage("canvas could not be created");
		return false;
	}
	
	CanvasMode mode = CANVAS_MODE_PAPER;
	CanvasPaperConfiguration configuration;
	configuration.pageResolution = OEMakeSize(612 * 2, 792 * 2);
	configuration.pagePixelDensity = OEMakeSize(144, 144);
	OEImage image;
	image.readFile(imagePath);
	
	canvas->postMessage(this, CANVAS_SET_MODE, &mode);
	canvas->postMessage(this, CANVAS_CONFIGURE_PAPER, &configuration);
	canvas->postMessage(this, CANVAS_POST_IMAGE, &image);
	
	return true;
}
