
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

bool AppleGraphicsTablet::setData(string name, OEData *data)
{
	if (name == "view")
	{
		
		delete data;
	}
	else
		return OEComponent::setData(name, data);
	
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
		CanvasFrame frame;
		frame.frameSize.width = 590;
		frame.frameSize.height = 606;
		frame.screenSize.width = 590;
		frame.screenSize.height = 606;
		
		canvas->postMessage(this,
							CANVAS_GET_FRAME,
							&frame);
		canvas->postMessage(this,
							CANVAS_RETURN_FRAME,
							&frame);
	}
	
	return true;
}
