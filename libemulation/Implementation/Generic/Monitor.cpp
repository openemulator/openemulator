
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
	
	configuration.size = OEMakeSize(720, 576);
	configuration.captureMode = CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK;
	screenRect = OEMakeRect(0, 0, 1, 1);
	
	audio = NULL;
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
	else if (name == "screenHorizontalCenter")
		screenRect.origin.x = getFloat(value);
	else if (name == "screenVerticalCenter")
		screenRect.origin.y = getFloat(value);
	else if (name == "screenHorizontalSize")
		screenRect.size.width = getFloat(value);
	else if (name == "screenVerticalSize")
		screenRect.size.height = getFloat(value);
	else if (name == "screenBarrel")
		configuration.screenBarrel = getFloat(value);
	else if (name == "screenScanlineAlpha")
		configuration.screenScanlineAlpha = getFloat(value);
	else if (name == "screenCenterLighting")
		configuration.screenCenterLighting = getFloat(value);
	else if (name == "screenShadowMaskAlpha")
		configuration.screenShadowMaskAlpha = getFloat(value);
	else if (name == "screenShadowMaskDotPitch")
		configuration.screenShadowMaskDotPitch = getFloat(value);
	else if (name == "screenShadowMask")
	{
		if (value == "Triad")
			configuration.screenShadowMask = CANVAS_SHADOWMASK_TRIAD;
		else if (value == "Inline")
			configuration.screenShadowMask = CANVAS_SHADOWMASK_INLINE;
		else if (value == "Aperture")
			configuration.screenShadowMask = CANVAS_SHADOWMASK_APERTURE;
	}
	else if (name == "screenPersistance")
		configuration.screenPersistance = getFloat(value);
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
	else if (name == "screenHorizontalCenter")
		value = getString(screenRect.origin.x);
	else if (name == "screenVerticalCenter")
		value = getString(screenRect.origin.y);
	else if (name == "screenHorizontalSize")
		value = getString(screenRect.size.width);
	else if (name == "screenVerticalSize")
		value = getString(screenRect.size.height);
	else if (name == "screenBarrel")
		value = getString(configuration.screenBarrel);
	else if (name == "screenScanlineAlpha")
		value = getString(configuration.screenScanlineAlpha);
	else if (name == "screenCenterLighting")
		value = getString(configuration.screenCenterLighting);
	else if (name == "screenShadowMaskAlpha")
		value = getString(configuration.screenShadowMaskAlpha);
	else if (name == "screenShadowMaskDotPitch")
		value = getString(configuration.screenShadowMaskDotPitch);
	else if (name == "screenShadowMask")
	{
		if (configuration.screenShadowMask == CANVAS_SHADOWMASK_TRIAD)
			value = "Triad";
		else if (configuration.screenShadowMask == CANVAS_SHADOWMASK_INLINE)
			value = "Inline";
		else if (configuration.screenShadowMask == CANVAS_SHADOWMASK_APERTURE)
			value = "Aperture";
	}
	else if (name == "screenPersistance")
		value = getString(configuration.screenPersistance);
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
			canvas->removeObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
			canvas->removeObserver(this, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
			canvas->removeObserver(this, CANVAS_POINTER_DID_CHANGE);
			canvas->removeObserver(this, CANVAS_MOUSE_DID_CHANGE);
			canvas->removeObserver(this, CANVAS_JOYSTICK1_DID_CHANGE);
			canvas->removeObserver(this, CANVAS_JOYSTICK2_DID_CHANGE);
			canvas->removeObserver(this, CANVAS_JOYSTICK3_DID_CHANGE);
			canvas->removeObserver(this, CANVAS_JOYSTICK4_DID_CHANGE);

			canvas->removeObserver(this, CANVAS_WILL_UPDATE);

			device->postMessage(this, DEVICE_DESTROY_CANVAS, &canvas);
		}
		device = ref;
		if (device)
		{
			device->postMessage(this, DEVICE_CREATE_CANVAS, &canvas);
			
			canvas->addObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
			canvas->addObserver(this, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
			canvas->addObserver(this, CANVAS_POINTER_DID_CHANGE);
			canvas->addObserver(this, CANVAS_MOUSE_DID_CHANGE);
			canvas->addObserver(this, CANVAS_JOYSTICK1_DID_CHANGE);
			canvas->addObserver(this, CANVAS_JOYSTICK2_DID_CHANGE);
			canvas->addObserver(this, CANVAS_JOYSTICK3_DID_CHANGE);
			canvas->addObserver(this, CANVAS_JOYSTICK4_DID_CHANGE);
			
			canvas->addObserver(this, CANVAS_WILL_UPDATE);
		}
	}
	else if (name == "audio")
	{
//		if (audio)
//			audio->removeObserver(this, AUDIO_FRAME_WILL_RENDER);
		audio = ref;
//		if (audio)
//			audio->addObserver(this, AUDIO_FRAME_WILL_RENDER);
	}
	else
		return false;
	
	return true;
}

bool Monitor::init()
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
	
	frame.readFile(dummyPath);
	
	canvas->postMessage(this, CANVAS_CONFIGURE, &configuration);
	canvas->postMessage(this, CANVAS_POST_FRAME, &frame);
	
	return true;
}

void Monitor::notify(OEComponent *sender, int notification, void *data)
{
	if (sender != canvas)
		return;
	
	if (notification != CANVAS_WILL_UPDATE)
		return;
	
	CanvasUpdate *update = (CanvasUpdate *)data;
	update->draw = true;
	
	int *p = (int *)frame.getPixels();
	if (p)
	{
		int w = 576;
		int h = 192;
		static int da = 0;
		for (int y = 0; y < h; y++)
			for (int x = 0; x < 512; x++)
				p[y * w + x] = ((x & 0x1f2) == da) ? 0xffffffff : 0x00000000;
		
		da += 0x11;
		da &= 0x1f0;
		
		canvas->postMessage(this, CANVAS_POST_FRAME, &frame);
	}
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
