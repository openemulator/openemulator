
/**
 * libemulation
 * Composite Monitor
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a composite monitor.
 */

#include "CompositeMonitor.h"
#include "Emulation.h"

CompositeMonitor::CompositeMonitor()
{
	emulation = NULL;
	canvas = NULL;
	
	configuration.zoomMode = CANVAS_ZOOMMODE_FIT_CANVAS;
	configuration.processMode = CANVAS_PROCESSMODE_COMPOSITE;
	configuration.captureMode = CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK;
	configuration.defaultViewSize = OEMakeSize(720, 576);
	configuration.canvasSize = OEMakeSize(720, 576);
	configuration.contentRect = OEMakeRect(0, 0, 1, 1);
	
	configuration.compositeDecoder = CANVAS_COMPOSITEDECODER_NTSC_YUV;
	configuration.compositeCarrierFrequency = 0.25;
	configuration.compositeLumaCutoffFrequency = 0.3;
	configuration.compositeChromaCutoffFrequency = 0.01;
	configuration.compositeBlackLevel = 0;
	configuration.compositeWhiteLevel = 1;
	configuration.compositeHue = 0.15;
	configuration.compositeScanlineAlpha = 0.2;
	
	configuration.brightness = 0;
	configuration.contrast = 1;
	configuration.saturation = 1;
	configuration.barrel = 0.1;
	configuration.persistance = 0;
	
	screenRect = OEMakeRect(0, 0, 1, 1);
}

bool CompositeMonitor::setValue(string name, string value)
{
	if (name == "compositeDecoder")
	{
		if (value == "CXA2025AS")
			 configuration.compositeDecoder = CANVAS_COMPOSITEDECODER_CXA2025AS;
		else if (value == "NTSC Y'IQ")
			configuration.compositeDecoder = CANVAS_COMPOSITEDECODER_NTSC_YIQ;
		else
			configuration.compositeDecoder = CANVAS_COMPOSITEDECODER_NTSC_YUV;
	}
	else if (name == "compositeCarrierFrequency")
		configuration.compositeCarrierFrequency = getFloat(value);
	else if (name == "compositeLumaCutoffFrequency")
		configuration.compositeLumaCutoffFrequency = getFloat(value);
	else if (name == "compositeChromaCutoffFrequency")
		configuration.compositeChromaCutoffFrequency = getFloat(value);
	else if (name == "compositeBlackLevel")
		configuration.compositeBlackLevel = getFloat(value);
	else if (name == "compositeWhiteLevel")
		configuration.compositeWhiteLevel = getFloat(value);
	else if (name == "compositeHue")
		configuration.compositeHue = getFloat(value);
	else if (name == "compositeScanlineAlpha")
		configuration.compositeScanlineAlpha = getFloat(value);
	else if (name == "brightness")
		configuration.brightness = getFloat(value);
	else if (name == "contrast")
		configuration.contrast = getFloat(value);
	else if (name == "saturation")
		configuration.saturation = getFloat(value);
	else if (name == "barrel")
		configuration.barrel = getFloat(value);
	else if (name == "persistance")
		configuration.persistance = getFloat(value);
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

bool CompositeMonitor::getValue(string name, string& value)
{
	if (name == "compositeDecoder")
	{
		if (configuration.compositeDecoder == CANVAS_COMPOSITEDECODER_CXA2025AS)
			value = "CXA2025AS";
		else if (configuration.compositeDecoder == CANVAS_COMPOSITEDECODER_NTSC_YIQ)
			value = "NTSC Y'IQ";
		else
			value = "NTSC Y'UV";
	}
	else if (name == "compositeCarrierFrequency")
		value = getString(configuration.compositeCarrierFrequency);
	else if (name == "compositeLumaCutoffFrequency")
		value = getString(configuration.compositeLumaCutoffFrequency);
	else if (name == "compositeChromaCutoffFrequency")
		value = getString(configuration.compositeChromaCutoffFrequency);
	else if (name == "compositeBlackLevel")
		value = getString(configuration.compositeBlackLevel);
	else if (name == "compositeWhiteLevel")
		value = getString(configuration.compositeWhiteLevel);
	else if (name == "compositeHue")
		value = getString(configuration.compositeHue);
	else if (name == "compositeScanlineAlpha")
		value = getString(configuration.compositeScanlineAlpha);
	else if (name == "brightness")
		value = getString(configuration.brightness);
	else if (name == "contrast")
		value = getString(configuration.contrast);
	else if (name == "saturation")
		value = getString(configuration.saturation);
	else if (name == "barrel")
		value = getString(configuration.barrel);
	else if (name == "persistance")
		value = getString(configuration.persistance);
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

bool CompositeMonitor::setRef(string name, OEComponent *ref)
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

bool CompositeMonitor::init()
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
		canvas->postMessage(this, CANVAS_CONFIGURE, &configuration);
		
		OEImage frame;
		frame.readFile(dummyPath);
		canvas->postMessage(this, CANVAS_POST_FRAME, &frame);
	}
	
	return true;
}

void CompositeMonitor::updateContentRect()
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

/*
 int *framebuffer = new int[framebufferWidth * framebufferHeight];
 int *p = framebuffer;
 
 for (int y = 0; y < framebufferHeight; y++)
 {
 float kk = 1;
 
 for (int x = 0; x < framebufferWidth; x++)
 {
 float l;
 //			float l = (((x >> 0) & 0x1) ^ ((y >> 1) & 0x1)) * 1.0;
 kk = 0.99 * kk;
 l = kk;
 
 if (y % 2)
 l *= 0.5;
 
 int r = l * 0x33;
 int g = l * 0xcc;
 int b = l * 0x44;
 *p++ = (b << 16) | (g << 8) | r;
 }
 }
 delete framebuffer;
 
 
 */