
/**
 * libemulation
 * Apple Graphics Tablet
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Graphics Tablet.
 */

#include "AppleGraphicsTablet.h"

#include "CompositeMonitor.h"
#include "Emulation.h"
#include "CanvasInterface.h"

AppleGraphicsTablet::AppleGraphicsTablet()
{
	emulation = NULL;
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
	if (name == "emulation")
	{
		if (emulation)
			emulation->postMessage(this,
								   EMULATION_DESTROY_CANVAS,
								   &canvas);
		emulation = ref;
		if (emulation)
			emulation->postMessage(this,
								   EMULATION_CREATE_CANVAS,
								   &canvas);
	}
	else
		return false;
	
	return true;
}

bool AppleGraphicsTablet::init()
{
	if (!emulation)
	{
		log("property 'emulation' undefined");
		return false;
	}
	
	if (!canvas)
	{
		log("canvas could not be created");
		return false;
	}
	
	if (canvas)
	{
		CanvasConfiguration configuration;
		configuration.size = OEMakeSize(637, 637);
		configuration.captureMode = CANVAS_CAPTUREMODE_NO_CAPTURE;
		canvas->postMessage(this, CANVAS_CONFIGURE, &configuration);
		
		OEImage *frame = NULL;
		canvas->postMessage(this, CANVAS_REQUEST_FRAME, &frame);
		if (frame)
			frame->readFile(viewPath);
		canvas->postMessage(this, CANVAS_RETURN_FRAME, &frame);
	}
	
	return true;
}
