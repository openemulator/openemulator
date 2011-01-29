
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
#include "CanvasInterface.h"

CompositeMonitor::CompositeMonitor()
{
	emulation = NULL;
	canvas = NULL;
}

bool CompositeMonitor::setValue(string name, string value)
{
	if (name == "compositeLumaCutoff")
		compositeLumaCutoff = getFloat(value);
	else if (name == "compositeChromaCutoff")
		compositeChromaCutoff = getFloat(value);
	else if (name == "compositeHue")
		compositeHue = getFloat(value);
	else if (name == "compositeSaturation")
		compositeSaturation = getFloat(value);
	else if (name == "compositeColorize")
		compositeColorize = getInt(value);
	else if (name == "compositeDecoderMatrix")
		compositeDecoderMatrix = getInt(value);
	else if (name == "screenBrightness")
		screenBrightness = getFloat(value);
	else if (name == "screenContrast")
		screenContrast = getFloat(value);
	else if (name == "screenRedGain")
		screenRedGain = getFloat(value);
	else if (name == "screenGreenGain")
		screenGreenGain = getFloat(value);
	else if (name == "screenBlueGain")
		screenBlueGain = getFloat(value);
	else if (name == "screenBarrel")
		screenBarrel = getFloat(value);
	else if (name == "screenPersistance")
		screenPersistance = getFloat(value);
	else if (name == "screenHorizontalCenter")
		screenHorizontalCenter = getFloat(value);
	else if (name == "screenHorizontalSize")
		screenHorizontalSize = getFloat(value);
	else if (name == "screenVerticalCenter")
		screenVerticalCenter = getFloat(value);
	else if (name == "screenVerticalSize")
		screenVerticalSize = getFloat(value);
	else if (name == "dummy")
		dummyPath = value;
	else
		return false;
	
	return true;
}

bool CompositeMonitor::getValue(string name, string &value)
{
	if (name == "compositeLumaCutoff")
		value = getString(compositeLumaCutoff);
	else if (name == "compositeChromaCutoff")
		value = getString(compositeChromaCutoff);
	else if (name == "compositeHue")
		value = getString(compositeHue);
	else if (name == "compositeSaturation")
		value = getString(compositeSaturation);
	else if (name == "compositeColorize")
		value = getString(compositeColorize);
	else if (name == "compositeDecoderMatrix")
		value = getString(compositeDecoderMatrix);
	else if (name == "screenBrightness")
		value = getString(screenBrightness);
	else if (name == "screenContrast")
		value = getString(screenContrast);
	else if (name == "screenRedGain")
		value = getString(screenRedGain);
	else if (name == "screenGreenGain")
		value = getString(screenGreenGain);
	else if (name == "screenBlueGain")
		value = getString(screenBlueGain);
	else if (name == "screenBarrel")
		value = getString(screenBarrel);
	else if (name == "screenPersistance")
		value = getString(screenPersistance);
	else if (name == "screenHorizontalCenter")
		value = getString(screenHorizontalCenter);
	else if (name == "screenHorizontalSize")
		value = getString(screenHorizontalSize);
	else if (name == "screenVerticalCenter")
		value = getString(screenVerticalCenter);
	else if (name == "screenVerticalSize")
		value = getString(screenVerticalSize);
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
		OEImage frame;
		frame.readFile(dummyPath);
		
		CanvasConfiguration configuration;
		configuration.viewMode = CANVAS_VIEWMODE_FIT_CANVAS;
		configuration.captureMode = CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK;
		configuration.defaultViewSize = OEMakeSize(768, 512);
		configuration.canvasSize = OEMakeSize(768, 512);
		configuration.contentRect = OEMakeRect(96.0 / 768, 52.0 / 483,
											   576.0 / 768, 192.0 * 2 / 483);
		canvas->postMessage(this, CANVAS_CONFIGURE, &configuration);
		
		canvas->postMessage(this, CANVAS_POST_FRAME, &frame);
	}
	
	return true;
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