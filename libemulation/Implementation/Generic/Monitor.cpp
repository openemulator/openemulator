
/**
 * libemulation
 * Monitor
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic monitor.
 */

#include "Monitor.h"
#include "Emulation.h"

Monitor::Monitor()
{
	emulation = NULL;
	canvas = NULL;
	
	screenRect = OEMakeRect(0, 0, 1, 1);
}

bool Monitor::setValue(string name, string value)
{
	if (name == "decoder")
	{
		if (value == "RGB")
			configuration.decoder = CANVAS_DECODER_RGB;
		else if (value == "Monochrome")
			configuration.decoder = CANVAS_DECODER_MONOCHROME;
		else if (value == "NTSC Y'IQ")
			configuration.decoder = CANVAS_DECODER_NTSC_YIQ;
		else if (value == "NTSC CXA2025AS")
			configuration.decoder = CANVAS_DECODER_NTSC_CXA2025AS;
		else if (value == "NTSC Y'UV")
			configuration.decoder = CANVAS_DECODER_NTSC_YUV;
		else if (value == "PAL")
			configuration.decoder = CANVAS_DECODER_PAL;
	}
	else if (name == "lumaBandwidth")
		configuration.lumaBandwidth = getFloat(value);
	else if (name == "brightness")
		configuration.brightness = getFloat(value);
	else if (name == "contrast")
		configuration.contrast = getFloat(value);
	else if (name == "saturation")
		configuration.saturation = getFloat(value);
	else if (name == "hue")
		configuration.hue = getFloat(value);
	else if (name == "barrel")
		configuration.barrel = getFloat(value);
	else if (name == "scanlineAlpha")
		configuration.scanlineAlpha = getFloat(value);
	else if (name == "shadowMaskAlpha")
		configuration.shadowMaskAlpha = getFloat(value);
	else if (name == "shadowMaskDotPitch")
		configuration.shadowMaskDotPitch = getFloat(value);
	else if (name == "shadowMask")
	{
		if (value == "Triad")
			configuration.shadowMask = CANVAS_SHADOWMASK_TRIAD;
		else if (value == "Inline")
			configuration.shadowMask = CANVAS_SHADOWMASK_INLINE;
		else if (value == "Aperture")
			configuration.shadowMask = CANVAS_SHADOWMASK_APERTURE;
	}
	else if (name == "centerLighting")
		configuration.centerLighting = getFloat(value);
	else if (name == "persistance")
		configuration.persistance = getFloat(value);
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
	else if (name == "horizontalCenter")
		screenRect.origin.x = getFloat(value);
	else if (name == "verticalCenter")
		screenRect.origin.y = getFloat(value);
	else if (name == "horizontalSize")
		screenRect.size.width = getFloat(value);
	else if (name == "verticalSize")
		screenRect.size.height = getFloat(value);
	else if (name == "dummy")
		dummyPath = value;
	else
		return false;
	
	updateContentRect();
	
	if (canvas)
		canvas->postMessage(this, CANVAS_CONFIGURE, &configuration);
	
	return true;
}

bool Monitor::getValue(string name, string& value)
{
	if (name == "decoder")
	{
		if (configuration.decoder == CANVAS_DECODER_RGB)
			value = "RGB";
		else if (configuration.decoder == CANVAS_DECODER_MONOCHROME)
			value = "Monochrome";
		else if (configuration.decoder == CANVAS_DECODER_NTSC_YIQ)
			value = "NTSC Y'IQ";
		else if (configuration.decoder == CANVAS_DECODER_NTSC_CXA2025AS)
			value = "NTSC CXA2025AS";
		else if (configuration.decoder == CANVAS_DECODER_NTSC_YUV)
			value = "NTSC Y'UV";
		else if (configuration.decoder == CANVAS_DECODER_PAL)
			value = "PAL";
		else
			value = "";
	}
	else if (name == "lumaBandwidth")
		value = getString(configuration.lumaBandwidth);
	else if (name == "brightness")
		value = getString(configuration.brightness);
	else if (name == "contrast")
		value = getString(configuration.contrast);
	else if (name == "saturation")
		value = getString(configuration.saturation);
	else if (name == "hue")
		value = getString(configuration.hue);
	else if (name == "barrel")
		value = getString(configuration.barrel);
	else if (name == "scanlineAlpha")
		value = getString(configuration.scanlineAlpha);
	else if (name == "shadowMaskAlpha")
		value = getString(configuration.shadowMaskAlpha);
	else if (name == "shadowMaskDotPitch")
		value = getString(configuration.shadowMaskDotPitch);
	else if (name == "shadowMask")
	{
		if (configuration.shadowMask == CANVAS_SHADOWMASK_TRIAD)
			value = "Triad";
		else if (configuration.shadowMask == CANVAS_SHADOWMASK_INLINE)
			value = "Inline";
		else if (configuration.shadowMask == CANVAS_SHADOWMASK_APERTURE)
			value = "Aperture";
		else
			value = "";
	}
	else if (name == "centerLighting")
		value = getString(configuration.centerLighting);
	else if (name == "persistance")
		value = getString(configuration.persistance);
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
	else if (name == "horizontalCenter")
		value = getString(screenRect.origin.x);
	else if (name == "verticalCenter")
		value = getString(screenRect.origin.y);
	else if (name == "horizontalSize")
		value = getString(screenRect.size.width);
	else if (name == "verticalSize")
		value = getString(screenRect.size.height);
	else
		return false;
	
	return true;
}

bool Monitor::setRef(string name, OEComponent *ref)
{
	if (name == "emulation")
	{
		if (emulation && canvas)
		{
			removeObserver(canvas, CANVAS_KEYBOARD_DID_CHANGE);
			removeObserver(canvas, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
			removeObserver(canvas, CANVAS_POINTER_DID_CHANGE);
			removeObserver(canvas, CANVAS_MOUSE_DID_CHANGE);
			removeObserver(canvas, CANVAS_JOYSTICK1_DID_CHANGE);
			removeObserver(canvas, CANVAS_JOYSTICK2_DID_CHANGE);
			removeObserver(canvas, CANVAS_JOYSTICK3_DID_CHANGE);
			removeObserver(canvas, CANVAS_JOYSTICK4_DID_CHANGE);
			emulation->postMessage(this,
								   EMULATION_DESTROY_CANVAS,
								   &canvas);
		}
		emulation = ref;
		if (emulation)
			emulation->postMessage(this,
								   EMULATION_CREATE_CANVAS,
								   &canvas);
		addObserver(canvas, CANVAS_KEYBOARD_DID_CHANGE);
		addObserver(canvas, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
		addObserver(canvas, CANVAS_POINTER_DID_CHANGE);
		addObserver(canvas, CANVAS_MOUSE_DID_CHANGE);
		addObserver(canvas, CANVAS_JOYSTICK1_DID_CHANGE);
		addObserver(canvas, CANVAS_JOYSTICK2_DID_CHANGE);
		addObserver(canvas, CANVAS_JOYSTICK3_DID_CHANGE);
		addObserver(canvas, CANVAS_JOYSTICK4_DID_CHANGE);
	}
	else
		return false;
	
	return true;
}

bool Monitor::init()
{
	if (!emulation)
	{
		logMessage("property 'emulation' undefined");
		return false;
	}

	if (!canvas)
	{
		logMessage("canvas could not be created");
		return false;
	}
	
	if (canvas)
	{
		canvas->postMessage(this, CANVAS_CONFIGURE, &configuration);
		
		OEImage frame;
		frame.readFile(dummyPath);
		canvas->postMessage(this, CANVAS_POST_FRAME, &frame);
	}
	
	return true;
}

void Monitor::updateContentRect()
{
	bool pal = false;
	if (pal)
		configuration.contentRect = OEMakeRect(96 * 1.0 / 768, 34 * 2.0 / 576,
											   576 * 1.0 / 768, 192 * 2.0 / 576);
	else
		configuration.contentRect = OEMakeRect(96 * 1.0 / 768, 24 * 2.0 / 483,
											   576 * 1.0 / 768, 192 * 2.0 / 483);
	
	configuration.contentRect.origin.x += (screenRect.origin.x +
										   configuration.contentRect.size.width *
										   (1.0 - screenRect.size.width) * 0.5);
	configuration.contentRect.origin.y += (screenRect.origin.y +
										   configuration.contentRect.size.height *
										   (1.0 - screenRect.size.height) * 0.5);
	configuration.contentRect.size.width *= screenRect.size.width;
	configuration.contentRect.size.height *= screenRect.size.height;
}
