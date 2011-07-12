
/**
 * libemulation
 * Monitor
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic monitor.
 */

#include "Monitor.h"

#include "DeviceInterface.h"
#include "AudioInterface.h"

Monitor::Monitor()
{
	device = NULL;
	canvas = NULL;
	
	configuration.displayResolution = OEMakeSize(768, 576);
	videoRect = OEMakeRect(0, 0, 1, 1);
}

bool Monitor::setValue(string name, string value)
{
	if (name == "videoDecoder")
	{
		if (value == "RGB")
			configuration.videoDecoder = CANVAS_DECODER_RGB;
		else if (value == "Monochrome")
			configuration.videoDecoder = CANVAS_DECODER_MONOCHROME;
		else if (value == "NTSC Y'IQ")
			configuration.videoDecoder = CANVAS_DECODER_NTSC_YIQ;
		else if (value == "NTSC CXA2025AS")
			configuration.videoDecoder = CANVAS_DECODER_NTSC_CXA2025AS;
		else if (value == "NTSC Y'UV")
			configuration.videoDecoder = CANVAS_DECODER_NTSC_YUV;
		else if (value == "PAL")
			configuration.videoDecoder = CANVAS_DECODER_PAL;
	}
	else if (name == "videoBandwidth")
		configuration.videoBandwidth = getFloat(value);
	else if (name == "videoBrightness")
		configuration.videoBrightness = getFloat(value);
	else if (name == "videoContrast")
		configuration.videoContrast = getFloat(value);
	else if (name == "videoSaturation")
		configuration.videoSaturation = getFloat(value);
	else if (name == "videoHue")
		configuration.videoHue = getFloat(value);
	else if (name == "videoHorizontalCenter")
		videoRect.origin.x = getFloat(value);
	else if (name == "videoVerticalCenter")
		videoRect.origin.y = getFloat(value);
	else if (name == "videoHorizontalSize")
		videoRect.size.width = getFloat(value);
	else if (name == "videoVerticalSize")
		videoRect.size.height = getFloat(value);
	else if (name == "displayBarrel")
		configuration.displayBarrel = getFloat(value);
	else if (name == "displayScanlineAlpha")
		configuration.displayScanlineAlpha = getFloat(value);
	else if (name == "displayCenterLighting")
		configuration.displayCenterLighting = getFloat(value);
	else if (name == "displayShadowMaskAlpha")
		configuration.displayShadowMaskAlpha = getFloat(value);
	else if (name == "displayShadowMaskDotPitch")
		configuration.displayShadowMaskDotPitch = getFloat(value);
	else if (name == "displayShadowMask")
	{
		if (value == "Triad")
			configuration.displayShadowMask = CANVAS_SHADOWMASK_TRIAD;
		else if (value == "Inline")
			configuration.displayShadowMask = CANVAS_SHADOWMASK_INLINE;
		else if (value == "Aperture")
			configuration.displayShadowMask = CANVAS_SHADOWMASK_APERTURE;
		else if (value == "LCD")
			configuration.displayShadowMask = CANVAS_SHADOWMASK_LCD;
		else if (value == "Bayer")
			configuration.displayShadowMask = CANVAS_SHADOWMASK_BAYER;
	}
	else if (name == "displayPersistance")
		configuration.displayPersistance = getFloat(value);
	else if (name == "compositeCarrierFrequency")
		configuration.compositeCarrierFrequency = getFloat(value);
	else if (name == "compositeLinePhase")
		configuration.compositeLinePhase = getFloat(value);
	else if (name == "compositeChromaBandwidth")
		configuration.compositeChromaBandwidth = getFloat(value);
	else if (name == "compositeBlackLevel")
		configuration.compositeBlackLevel = getFloat(value);
	else if (name == "compositeWhiteLevel")
		configuration.compositeWhiteLevel = getFloat(value);
	else
		return false;
	
	return true;
}

bool Monitor::getValue(string name, string& value)
{
	if (name == "videoDecoder")
	{
		if (configuration.videoDecoder == CANVAS_DECODER_RGB)
			value = "RGB";
		else if (configuration.videoDecoder == CANVAS_DECODER_MONOCHROME)
			value = "Monochrome";
		else if (configuration.videoDecoder == CANVAS_DECODER_NTSC_YIQ)
			value = "NTSC Y'IQ";
		else if (configuration.videoDecoder == CANVAS_DECODER_NTSC_CXA2025AS)
			value = "NTSC CXA2025AS";
		else if (configuration.videoDecoder == CANVAS_DECODER_NTSC_YUV)
			value = "NTSC Y'UV";
		else if (configuration.videoDecoder == CANVAS_DECODER_PAL)
			value = "PAL";
	}
	else if (name == "videoBandwidth")
		value = getString(configuration.videoBandwidth);
	else if (name == "videoBrightness")
		value = getString(configuration.videoBrightness);
	else if (name == "videoContrast")
		value = getString(configuration.videoContrast);
	else if (name == "videoSaturation")
		value = getString(configuration.videoSaturation);
	else if (name == "videoHue")
		value = getString(configuration.videoHue);
	else if (name == "videoHorizontalCenter")
		value = getString(videoRect.origin.x);
	else if (name == "videoVerticalCenter")
		value = getString(videoRect.origin.y);
	else if (name == "videoHorizontalSize")
		value = getString(videoRect.size.width);
	else if (name == "videoVerticalSize")
		value = getString(videoRect.size.height);
	else if (name == "displayBarrel")
		value = getString(configuration.displayBarrel);
	else if (name == "displayScanlineAlpha")
		value = getString(configuration.displayScanlineAlpha);
	else if (name == "displayCenterLighting")
		value = getString(configuration.displayCenterLighting);
	else if (name == "displayShadowMaskAlpha")
		value = getString(configuration.displayShadowMaskAlpha);
	else if (name == "displayShadowMaskDotPitch")
		value = getString(configuration.displayShadowMaskDotPitch);
	else if (name == "displayShadowMask")
	{
		if (configuration.displayShadowMask == CANVAS_SHADOWMASK_TRIAD)
			value = "Triad";
		else if (configuration.displayShadowMask == CANVAS_SHADOWMASK_INLINE)
			value = "Inline";
		else if (configuration.displayShadowMask == CANVAS_SHADOWMASK_APERTURE)
			value = "Aperture";
		else if (configuration.displayShadowMask == CANVAS_SHADOWMASK_LCD)
			value = "LCD";
		else if (configuration.displayShadowMask == CANVAS_SHADOWMASK_BAYER)
			value = "Bayer";
	}
	else if (name == "displayPersistance")
		value = getString(configuration.displayPersistance);
	else if (name == "compositeBlackLevel")
		value = getString(configuration.compositeBlackLevel);
	else if (name == "compositeWhiteLevel")
		value = getString(configuration.compositeWhiteLevel);
	else if (name == "compositeCarrierFrequency")
		value = getString(configuration.compositeCarrierFrequency);
	else if (name == "compositeLinePhase")
		value = getString(configuration.compositeLinePhase);
	else if (name == "compositeChromaBandwidth")
		value = getString(configuration.compositeChromaBandwidth);
	else
		return false;
	
	return true;
}

bool Monitor::setRef(string name, OEComponent *ref)
{
	if (name == "device")
	{
		if (device)
		{
			if (canvas)
			{
				canvas->removeObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
				canvas->removeObserver(this, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
				canvas->removeObserver(this, CANVAS_POINTER_DID_CHANGE);
				canvas->removeObserver(this, CANVAS_MOUSE_DID_CHANGE);
				canvas->removeObserver(this, CANVAS_JOYSTICK1_DID_CHANGE);
				canvas->removeObserver(this, CANVAS_JOYSTICK2_DID_CHANGE);
				canvas->removeObserver(this, CANVAS_JOYSTICK3_DID_CHANGE);
				canvas->removeObserver(this, CANVAS_JOYSTICK4_DID_CHANGE);
				
				canvas->removeObserver(this, CANVAS_DID_COPY);
				canvas->removeObserver(this, CANVAS_DID_PASTE);
			}
			
			device->postMessage(this, DEVICE_DESTROY_CANVAS, &canvas);
		}
		device = ref;
		if (device)
		{
			device->postMessage(this, DEVICE_CONSTRUCT_CANVAS, &canvas);
			
			if (canvas)
			{
				canvas->addObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
				canvas->addObserver(this, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
				canvas->addObserver(this, CANVAS_POINTER_DID_CHANGE);
				canvas->addObserver(this, CANVAS_MOUSE_DID_CHANGE);
				canvas->addObserver(this, CANVAS_JOYSTICK1_DID_CHANGE);
				canvas->addObserver(this, CANVAS_JOYSTICK2_DID_CHANGE);
				canvas->addObserver(this, CANVAS_JOYSTICK3_DID_CHANGE);
				canvas->addObserver(this, CANVAS_JOYSTICK4_DID_CHANGE);
				
				canvas->addObserver(this, CANVAS_DID_COPY);
				canvas->addObserver(this, CANVAS_DID_PASTE);
			}
		}
	}
	else
		return false;
	
	return true;
}

bool Monitor::init()
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
	
	return true;
}

void Monitor::update()
{
	updateVideoRect();
	
	if (canvas)
		canvas->postMessage(this, CANVAS_CONFIGURE_DISPLAY, &configuration);
}

bool Monitor::postMessage(OEComponent *sender, int message, void *data)
{
    if (canvas)
    {
        if (message == CANVAS_POST_IMAGE)
        {
            OEImage *image = (OEImage *)data;
            
            int options = image->getOptions();
            
            options = 0;
            
            // Analyze image->getOptions(), set saturation and bandwidth accordingly
        }
        return canvas->postMessage(sender, message, data);
    }
    
    return false;
}

void Monitor::updateVideoRect()
{
	bool pal = false;
	if (pal)
		configuration.videoRect = OEMakeRect(96 * 1.0 / 768, 34 * 2.0 / 576,
											 576 * 1.0 / 768, 192 * 2.0 / 576);
	else
		configuration.videoRect = OEMakeRect(96 * 1.0 / 768, 24 * 2.0 / 483,
											 576 * 1.0 / 768, 192 * 2.0 / 483);
	
	configuration.videoRect.origin.x += (videoRect.origin.x +
										 configuration.videoRect.size.width *
										 (1.0 - videoRect.size.width) * 0.5);
	configuration.videoRect.origin.y += (videoRect.origin.y +
										 configuration.videoRect.size.height *
										 (1.0 - videoRect.size.height) * 0.5);
	configuration.videoRect.size.width *= videoRect.size.width;
	configuration.videoRect.size.height *= videoRect.size.height;
}
