
/**
 * libemulation
 * Apple Graphics Tablet
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Graphics Tablet
 */

#include "AppleGraphicsTablet.h"

#include "DeviceInterface.h"
#include "CanvasInterface.h"

#include "AppleIIInterface.h"

AppleGraphicsTablet::AppleGraphicsTablet()
{
	device = NULL;
	canvas = NULL;
    io = NULL;
}

bool AppleGraphicsTablet::setValue(string name, string value)
{
	if (name == "imagePath")
		imagePath = value;
	else
		return false;
	
	return true;
}

bool AppleGraphicsTablet::setRef(string name, OEComponent *ref)
{
	if (name == "device")
	{
		if (device)
        {
            if (canvas)
                canvas->removeObserver(this, CANVAS_POINTER_DID_CHANGE);
            
			device->postMessage(this,
                                DEVICE_DESTROY_CANVAS,
                                &canvas);
        }
        
		device = ref;
		if (device)
        {
			device->postMessage(this,
                                DEVICE_CONSTRUCT_CANVAS,
                                &canvas);
            
            if (canvas)
                canvas->addObserver(this, CANVAS_POINTER_DID_CHANGE);
        }
	}
	else if (name == "io")
        io = ref;
    else
		return false;
	
	return true;
}

bool AppleGraphicsTablet::init()
{
	if (!device)
	{
		logMessage("device not connected");
        
		return false;
	}
	
	if (!canvas)
	{
		logMessage("canvas could not be created");
        
		return false;
	}
    
    if (!io)
    {
		logMessage("io not connected");
        
		return false;
    }
	
	CanvasDisplayConfiguration configuration;
	OEImage image;
	image.load(imagePath);
	
	configuration.displayResolution = image.getSize();
	configuration.displayPixelDensity = 50;
	canvas->postMessage(this, CANVAS_CONFIGURE_DISPLAY, &configuration);
	
	canvas->postMessage(this, CANVAS_POST_IMAGE, &image);
	
	return true;
}

void AppleGraphicsTablet::notify(OEComponent *sender, int notification, void *data)
{
    postNotification(sender, notification, data);
}
