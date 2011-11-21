
/**
 * libemulation
 * Apple Hand Controllers
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple Hand Controllers
 */

#include "AppleHandControllers.h"

#include "AppleIIInterface.h"
#include "CanvasInterface.h"
#include "JoystickInterface.h"

#define JOYSTICK_OFFSET 0x100

AppleHandControllers::AppleHandControllers()
{
    gamePort = NULL;
    video = NULL;
    joystick = NULL;
    
    nameMap["Mouse X"] = CANVAS_M_RX;
    nameMap["Mouse Y"] = CANVAS_M_RY;
    nameMap["Mouse Wheel X"] = CANVAS_M_WHEELX;
    nameMap["Mouse Wheel Y"] = CANVAS_M_WHEELY;
    nameMap["Mouse Button 1"] = CANVAS_M_BUTTON1;
    nameMap["Mouse Button 2"] = CANVAS_M_BUTTON2;
    nameMap["Mouse Button 3"] = CANVAS_M_BUTTON3;
    nameMap["Mouse Button 4"] = CANVAS_M_BUTTON4;
    
    nameMap["Joystick 1 X-Axis"] = JOYSTICK_OFFSET + CANVAS_J_X;
    nameMap["Joystick 1 Y-Axis"] = JOYSTICK_OFFSET + CANVAS_J_Y;
    nameMap["Joystick 1 Z-Axis"] = JOYSTICK_OFFSET + CANVAS_J_Z;
    nameMap["Joystick 1 X-Rotation"] = JOYSTICK_OFFSET + CANVAS_J_RX;
    nameMap["Joystick 1 Y-Rotation"] = JOYSTICK_OFFSET + CANVAS_J_RY;
    nameMap["Joystick 1 Z-Rotation"] = JOYSTICK_OFFSET + CANVAS_J_RZ;
    nameMap["Joystick 1 Slider"] = JOYSTICK_OFFSET + CANVAS_J_SLIDER;
    nameMap["Joystick 1 Dial"] = JOYSTICK_OFFSET + CANVAS_J_DIAL;
    nameMap["Joystick 1 Wheel"] = JOYSTICK_OFFSET + CANVAS_J_WHEEL;
    nameMap["Joystick 1 Button 1"] = JOYSTICK_OFFSET + CANVAS_J_BUTTON1;
    nameMap["Joystick 1 Button 2"] = JOYSTICK_OFFSET + CANVAS_J_BUTTON2;
    nameMap["Joystick 1 Button 3"] = JOYSTICK_OFFSET + CANVAS_J_BUTTON3;
    nameMap["Joystick 1 Button 4"] = JOYSTICK_OFFSET + CANVAS_J_BUTTON4;
    nameMap["Joystick 1 Button 5"] = JOYSTICK_OFFSET + CANVAS_J_BUTTON5;
    nameMap["Joystick 1 Button 6"] = JOYSTICK_OFFSET + CANVAS_J_BUTTON6;
    nameMap["Joystick 1 Button 7"] = JOYSTICK_OFFSET + CANVAS_J_BUTTON7;
    nameMap["Joystick 1 Button 8"] = JOYSTICK_OFFSET + CANVAS_J_BUTTON8;

    nameMap["Joystick 2 X-Axis"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_X;
    nameMap["Joystick 2 Y-Axis"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_Y;
    nameMap["Joystick 2 Z-Axis"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_Z;
    nameMap["Joystick 2 X-Rotation"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_RX;
    nameMap["Joystick 2 Y-Rotation"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_RY;
    nameMap["Joystick 2 Z-Rotation"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_RZ;
    nameMap["Joystick 2 Slider"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_SLIDER;
    nameMap["Joystick 2 Dial"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_DIAL;
    nameMap["Joystick 2 Wheel"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_WHEEL;
    nameMap["Joystick 2 Button 1"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_BUTTON1;
    nameMap["Joystick 2 Button 2"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_BUTTON2;
    nameMap["Joystick 2 Button 3"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_BUTTON3;
    nameMap["Joystick 2 Button 4"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_BUTTON4;
    nameMap["Joystick 2 Button 5"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_BUTTON5;
    nameMap["Joystick 2 Button 6"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_BUTTON6;
    nameMap["Joystick 2 Button 7"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_BUTTON7;
    nameMap["Joystick 2 Button 8"] = JOYSTICK_OFFSET + CANVAS_JOYSTICK_BLOCK_SIZE + CANVAS_J_BUTTON8;
    
    nameMap["Unmapped"] = -1;
    
    AppleHandControllersConf preset;
    
    preset.pdl[0] = nameMap["Mouse X"];
    preset.reversePDL[0] = false;
    preset.pdl[1] = nameMap["Mouse Y"];
    preset.reversePDL[1] = false;
    preset.pdl[2] = nameMap["Unmapped"];
    preset.reversePDL[2] = false;
    preset.pdl[3] = nameMap["Unmapped"];
    preset.reversePDL[3] = false;
    preset.pb[0] = nameMap["Mouse Button 1"];
    preset.pb[1] = nameMap["Mouse Button 2"];
    preset.pb[2] = nameMap["Mouse Button 3"];
    presets["Mouse"] = preset;
    
    preset.pdl[0] = nameMap["Joystick 1 X-Axis"];
    preset.reversePDL[0] = false;
    preset.pdl[1] = nameMap["Joystick 1 Y-Axis"];
    preset.reversePDL[1] = false;
    preset.pdl[2] = nameMap["Joystick 1 Z-Rotation"];
    preset.reversePDL[2] = false;
    preset.pdl[3] = nameMap["Joystick 1 Slider"];
    preset.reversePDL[3] = false;
    preset.pb[0] = nameMap["Joystick 1 Button 1"];
    preset.pb[1] = nameMap["Joystick 1 Button 2"];
    preset.pb[2] = nameMap["Joystick 1 Button 3"];
    presets["Joystick"] = preset;
    
    for (int i = 0; i < 4; i++)
    {
        conf.pdl[i] = -1;
        conf.reversePDL[i] = false;
    }
    for (int i = 0; i < 4; i++)
        conf.pb[i] = -1;
}

bool AppleHandControllers::setValue(string name, string value)
{
    if (name == "configuration")
        setPreset(value);
    else if (name == "pdl0")
        setPDL(0, value);
    else if (name == "reversePDL0")
        conf.reversePDL[0] = getInt(value);
    else if (name == "pdl1")
        setPDL(1, value);
    else if (name == "reversePDL1")
        conf.reversePDL[1] = getInt(value);
    else if (name == "pdl2")
        setPDL(2, value);
    else if (name == "reversePDL2")
        conf.reversePDL[2] = getInt(value);
    else if (name == "pdl3")
        setPDL(3, value);
    else if (name == "reversePDL3")
        conf.reversePDL[3] = getInt(value);
    else if (name == "pb0")
        setPB(0, value);
    else if (name == "pb1")
        setPB(1, value);
    else if (name == "pb2")
        setPB(2, value);
    else
        return false;
    
    return true;
}

bool AppleHandControllers::getValue(string name, string &value)
{
    if (name == "configuration")
        value = getPreset();
    else if (name == "pdl0")
        value = getPDL(0);
    else if (name == "reversePDL0")
        value = getString(conf.reversePDL[0]);
    else if (name == "pdl1")
        value = getPDL(1);
    else if (name == "reversePDL1")
        value = getString(conf.reversePDL[1]);
    else if (name == "pdl2")
        value = getPDL(2);
    else if (name == "reversePDL2")
        value = getString(conf.reversePDL[2]);
    else if (name == "pdl3")
        value = getPDL(3);
    else if (name == "reversePDL3")
        value = getString(conf.reversePDL[3]);
    else if (name == "pb0")
        value = getPB(0);
    else if (name == "pb1")
        value = getPB(1);
    else if (name == "pb2")
        value = getPB(2);
    else
        return false;
    
    return true;
}

bool AppleHandControllers::setRef(string name, OEComponent *ref)
{
    if (name == "gamePort")
        gamePort = ref;
    else if (name == "video")
    {
        if (video)
            video->removeObserver(this, APPLEIIVIDEO_MOUSE_DID_CHANGE);
        video = ref;
        if (video)
            video->addObserver(this, APPLEIIVIDEO_MOUSE_DID_CHANGE);
    }
    else if (name == "joystick")
    {
        if (joystick)
            joystick->removeObserver(this, JOYSTICK_DID_CHANGE);
        joystick = ref;
        if (joystick)
            joystick->addObserver(this, JOYSTICK_DID_CHANGE);
    }
    else
        return false;
    
    return true;
}

void AppleHandControllers::notify(OEComponent *sender, int notification, void *data)
{
    CanvasHIDNotification *hidNotification = (CanvasHIDNotification *)data;
    OEUInt32 usageId = hidNotification->usageId;
    
    if (sender == joystick)
        usageId += JOYSTICK_OFFSET;
    
    for (int i = 0; i < 4; i++)
    {
        if (usageId == conf.pdl[i])
            gamePort->postMessage(this, APPLEIIGAMEPORT_SET_PDL0 + i, &hidNotification->value);
    }
    for (int i = 0; i < 3; i++)
    {
        if (usageId == conf.pb[i])
            gamePort->postMessage(this, APPLEIIGAMEPORT_SET_PB0 + i, &hidNotification->value);
    }
}

void AppleHandControllers::setPreset(string name)
{
    if (name == "Customized")
        return;
    
    conf = presets[name];
}

string AppleHandControllers::getPreset()
{
    for (map<string, AppleHandControllersConf>::iterator i = presets.begin();
         i != presets.end();
         i++)
    {
        AppleHandControllersConf c = i->second;
        if ((c.pdl[0] == conf.pdl[0]) &&
            (c.reversePDL[0] == conf.reversePDL[0]) &&
            (c.pdl[1] == conf.pdl[1]) &&
            (c.reversePDL[1] == conf.reversePDL[1]) &&
            (c.pdl[2] == conf.pdl[2]) &&
            (c.reversePDL[2] == conf.reversePDL[2]) &&
            (c.pdl[3] == conf.pdl[3]) &&
            (c.reversePDL[3] == conf.reversePDL[3]) &&
            (c.pb[0] == conf.pb[0]) &&
            (c.pb[1] == conf.pb[1]) &&
            (c.pb[2] == conf.pb[2]))
            return i->first;
    }
    
    return "Customized";
}

void AppleHandControllers::setPDL(int index, string value)
{
    conf.pdl[index] = -1;
    
    for (map<string,OEUInt32>::iterator i = nameMap.begin();
         i != nameMap.end();
         i++)
    {
        if (value == i->first)
            conf.pdl[index] = i->second;
    }
}

string AppleHandControllers::getPDL(int index)
{
    for (map<string,OEUInt32>::iterator i = nameMap.begin();
         i != nameMap.end();
         i++)
    {
        if (conf.pdl[index] == i->second)
            return i->first;
    }
    
    return "";
}

void AppleHandControllers::setPB(int index, string value)
{
    conf.pb[index] = -1;
    
    for (map<string,OEUInt32>::iterator i = nameMap.begin();
         i != nameMap.end();
         i++)
    {
        if (value == i->first)
            conf.pb[index] = i->second;
    }
}

string AppleHandControllers::getPB(int index)
{
    for (map<string,OEUInt32>::iterator i = nameMap.begin();
         i != nameMap.end();
         i++)
    {
        if (conf.pb[index] == i->second)
            return i->first;
    }
    
    return "";
}
