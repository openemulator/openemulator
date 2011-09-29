
/**
 * libemulation
 * Monitor
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic monitor
 */

#include "Monitor.h"

#include "DeviceInterface.h"
#include "AudioInterface.h"

Monitor::Monitor()
{
    device = NULL;
    controlBus = NULL;
    canvas = NULL;
    
    powerState = CONTROLBUS_POWERSTATE_ON;
    
    configuration.displayResolution = OEMakeSize(768, 576);
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
        configuration.videoCenter.x = getFloat(value);
    else if (name == "videoVerticalCenter")
        configuration.videoCenter.y = getFloat(value);
    else if (name == "videoHorizontalSize")
        configuration.videoSize.width = getFloat(value);
    else if (name == "videoVerticalSize")
        configuration.videoSize.height = getFloat(value);
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
    else if (name == "compositeLumaBandwidth")
        configuration.compositeLumaBandwidth = getFloat(value);
    else if (name == "compositeChromaBandwidth")
        configuration.compositeChromaBandwidth = getFloat(value);
    else if (name == "compositeChromaCarrier")
        configuration.compositeChromaCarrier = getFloat(value);
    else if (name == "compositeChromaLine")
        configuration.compositeChromaLine = getFloat(value);
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
        value = getString(configuration.videoCenter.x);
    else if (name == "videoVerticalCenter")
        value = getString(configuration.videoCenter.y);
    else if (name == "videoHorizontalSize")
        value = getString(configuration.videoSize.width);
    else if (name == "videoVerticalSize")
        value = getString(configuration.videoSize.height);
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
    else if (name == "compositeLumaBandwidth")
        value = getString(configuration.compositeLumaBandwidth);
    else if (name == "compositeChromaBandwidth")
        value = getString(configuration.compositeChromaBandwidth);
    else if (name == "compositeChromaCarrier")
        value = getString(configuration.compositeChromaCarrier);
    else if (name == "compositeChromaLine")
        value = getString(configuration.compositeChromaLine);
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
                canvas->removeObserver(this, CANVAS_UNICODECHAR_WAS_SENT);
                canvas->removeObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
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
                canvas->addObserver(this, CANVAS_UNICODECHAR_WAS_SENT);
                canvas->addObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
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
    else if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
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
    
    if (controlBus)
        controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    return true;
}

void Monitor::update()
{
    if (canvas)
        canvas->postMessage(this, CANVAS_CONFIGURE_DISPLAY, &configuration);
}

bool Monitor::postMessage(OEComponent *sender, int message, void *data)
{
    if (canvas && (powerState != CONTROLBUS_POWERSTATE_OFF))
        return canvas->postMessage(sender, message, data);
    
    return false;
}

void Monitor::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        CanvasBezel bezel;
        
        powerState = *((ControlBusPowerState *)data);
        
        switch(powerState)
        {
            case CONTROLBUS_POWERSTATE_OFF:
                canvas->postMessage(this, CANVAS_CLEAR, NULL);
                
                bezel = CANVAS_BEZEL_POWER;
                
                break;
                
            case CONTROLBUS_POWERSTATE_ON:
                bezel = CANVAS_BEZEL_NONE;
                
                break;
                
            default:
                bezel = CANVAS_BEZEL_PAUSE;
                
                break;
        }
        
        canvas->postMessage(this, CANVAS_SET_BEZEL, &bezel);
    }
    else
        OEComponent::notify(sender, notification, data);
}
