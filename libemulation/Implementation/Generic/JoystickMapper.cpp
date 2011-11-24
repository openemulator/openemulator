
/**
 * libemulation
 * Joystick Mapper
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a Joystick Mapper
 */

#include <math.h>

#include "JoystickMapper.h"

#include "DeviceInterface.h"
#include "CanvasInterface.h"
#include "JoystickInterface.h"

#define MOUSE_OFFSET 0x100
#define JOYSTICK_OFFSET 0x100

JoystickMapper::JoystickMapper()
{
    device = NULL;
    canvas = NULL;
    joystick = NULL;
    
    addItem("Mouse", "Mouse X", JOYSTICKMAPPER_RELATIVEAXIS, CANVAS_M_RX);
    addItem("Mouse", "Mouse Y", JOYSTICKMAPPER_RELATIVEAXIS, CANVAS_M_RY);
    addItem("Mouse", "Mouse Wheel X", JOYSTICKMAPPER_RELATIVEAXIS, CANVAS_M_WHEELX);
    addItem("Mouse", "Mouse Wheel Y", JOYSTICKMAPPER_RELATIVEAXIS, CANVAS_M_WHEELY);
    for (int i = 0; i < CANVAS_MOUSE_BUTTON_NUM; i++)
    {
        string label = "Mouse Button " + getString(i + 1);
        
        addItem("Mouse", label, JOYSTICKMAPPER_BUTTON, CANVAS_M_BUTTON1 + i);
    }
    addItem("Mouse", "Unmapped", JOYSTICKMAPPER_UNMAPPED, -1);
    
    for (int deviceId = 0; deviceId < 2; deviceId++)
    {
        string deviceLabel = "Joystick " + getString(deviceId + 1);
        int deviceOffset = (deviceId + 1) * JOYSTICK_OFFSET;
        
        for (int i = 0; i < JOYSTICK_AXIS_NUM; i++)
        {
            addItem("Joystick", deviceLabel + " X-Axis", JOYSTICKMAPPER_AXIS, deviceOffset + JOYSTICK_AXIS1);
            addItem("Joystick", deviceLabel + " Y-Axis", JOYSTICKMAPPER_AXIS, deviceOffset + JOYSTICK_AXIS2);
            addItem("Joystick", deviceLabel + " Z-Axis", JOYSTICKMAPPER_AXIS, deviceOffset + JOYSTICK_AXIS3);
            addItem("Joystick", deviceLabel + " X-Rotation", JOYSTICKMAPPER_AXIS, deviceOffset + JOYSTICK_AXIS4);
            addItem("Joystick", deviceLabel + " Y-Rotation", JOYSTICKMAPPER_AXIS, deviceOffset + JOYSTICK_AXIS5);
            addItem("Joystick", deviceLabel + " Z-Rotation", JOYSTICKMAPPER_AXIS, deviceOffset + JOYSTICK_AXIS6);
            addItem("Joystick", deviceLabel + " Slider", JOYSTICKMAPPER_AXIS, deviceOffset + JOYSTICK_AXIS7);
            addItem("Joystick", deviceLabel + " Dial", JOYSTICKMAPPER_AXIS, deviceOffset + JOYSTICK_AXIS8);
            addItem("Joystick", deviceLabel + " Wheel", JOYSTICKMAPPER_AXIS, deviceOffset + JOYSTICK_AXIS9);
        }
        for (int i = 0; i < JOYSTICK_BUTTON_NUM; i++)
        {
            string label = deviceLabel + " Button " + getString(i + 1);
            
            addItem("Joystick", label, JOYSTICKMAPPER_BUTTON, deviceOffset + JOYSTICK_BUTTON1 + i);
        }
    }
    addItem("Joystick", "Unmapped", JOYSTICKMAPPER_UNMAPPED, -1);
    
    addItem("None", "Unmapped", JOYSTICKMAPPER_UNMAPPED, -1);
    
    deviceId = 0;
}

bool JoystickMapper::setValue(string name, string value)
{
    if (name == "inputDevice")
        inputDevice = value;
    else if (name == "deviceId")
        deviceId = (OEUInt32) getInt(value);
    else if (name.substr(0, 4) == "axis")
        setUsageId(JOYSTICK_AXIS1 + (OEInt32) getInt(name.substr(4)), value);
    else if (name.substr(0, 6) == "button")
        setUsageId(JOYSTICK_BUTTON1 + (OEInt32) getInt(name.substr(6)), value);
    else if (name.substr(0, 15) == "sensitivityAxis")
        setSensitivity(JOYSTICK_AXIS1 + (OEInt32) getInt(name.substr(15)),
                       getFloat(value));
    else if (name.substr(0, 11) == "reverseAxis")
        setReverse(JOYSTICK_AXIS1 + (OEInt32) getInt(name.substr(11)),
                   getInt(value));
    else if (name.substr(0, 3) == "map")
        inputDeviceMap[name.substr(3)] = value;
    else
        return false;
    
    return true;
}

bool JoystickMapper::getValue(string name, string &value)
{
    if (name == "inputDevice")
        value = inputDevice;
    else if (name == "deviceId")
        value = getString(deviceId);
    else if (name.substr(0, 4) == "axis")
        value = getUsageId(JOYSTICK_AXIS1 + (OEUInt32) getInt(name.substr(4)));
    else if (name.substr(0, 6) == "button")
        value = getUsageId(JOYSTICK_BUTTON1 + (OEUInt32) getInt(name.substr(6)));
    else if (name.substr(0, 15) == "sensitivityAxis")
        value = getSensitivity(JOYSTICK_AXIS1 + (OEUInt32) getInt(name.substr(15)));
    else if (name.substr(0, 11) == "reverseAxis")
        value = getReverse(JOYSTICK_AXIS1 + (OEUInt32) getInt(name.substr(11)));
    else if (name.substr(0, 3) == "map")
        value = inputDeviceMap[name.substr(3)];
    else
        return false;
    
    return true;
}

bool JoystickMapper::setRef(string name, OEComponent *ref)
{
    if (name == "device")
        device = ref;
    else if (name == "canvas")
    {
        if (canvas)
            canvas->removeObserver(this, CANVAS_MOUSE_DID_CHANGE);
        canvas = ref;
        if (canvas)
            canvas->addObserver(this, CANVAS_MOUSE_DID_CHANGE);
    }
    else if (name == "joystick")
    {
        if (joystick)
        {
            joystick->removeObserver(this, JOYSTICK_DID_CHANGE);
            joystick->removeObserver(this, JOYSTICK_WAS_ADDED);
            joystick->removeObserver(this, JOYSTICK_WAS_REMOVED);
        }
        joystick = ref;
        if (joystick)
        {
            joystick->addObserver(this, JOYSTICK_DID_CHANGE);
            joystick->addObserver(this, JOYSTICK_WAS_ADDED);
            joystick->addObserver(this, JOYSTICK_WAS_REMOVED);
        }
    }
    else
        return false;
    
    return true;
}

bool JoystickMapper::init()
{
    if (!device)
    {
        logMessage("device not connected");
        
        return false;
    }
    
    return true;
}

void JoystickMapper::update()
{
    if (inputDevice != oldInputDevice)
    {
        vector<string> axes;
        vector<string> buttons;
        
        for (JoystickMapperItems::iterator i = items.begin();
             i != items.end();
             i++)
        {
            if (i->inputDevice == inputDevice)
            {
                switch (i->type)
                {
                    case JOYSTICKMAPPER_AXIS:
                    case JOYSTICKMAPPER_RELATIVEAXIS:
                        axes.push_back(i->label);
                        
                        break;
                        
                    case JOYSTICKMAPPER_BUTTON:
                        buttons.push_back(i->label);
                        
                        break;
                        
                    case JOYSTICKMAPPER_UNMAPPED:
                        axes.push_back(i->label);
                        buttons.push_back(i->label);
                        
                        break;
                        
                    default:
                        break;
                }
            }
        }
        
        DeviceSettings settings;
        
        device->postMessage(this, DEVICE_GET_SETTINGS, &settings);
        
        for (DeviceSettings::iterator i = settings.begin();
             i != settings.end();
             i++)
        {
            if ((i->name.substr(0, 4) == "axis") && (i->type == "select"))
                i->options = strjoin(axes, ',');
            if ((i->name.substr(0, 6) == "button") && (i->type == "select"))
                i->options = strjoin(buttons, ',');
        }
        
        device->postMessage(this, DEVICE_SET_SETTINGS, &settings);
        
        inputDeviceMap[oldInputDevice] = serializeMap();
        
        device->postMessage(this, DEVICE_UPDATE, NULL);
        
        unserializeMap(inputDeviceMap[inputDevice]);
        
        oldInputDevice = inputDevice;
    }
}

void JoystickMapper::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == canvas)
    {
        CanvasHIDNotification *hidNotification = (CanvasHIDNotification *)data;
        
        mapNotification(hidNotification->usageId, hidNotification->value);
    }
    else if (sender == joystick)
    {
        if (notification == JOYSTICK_DID_CHANGE)
        {
            JoystickHIDNotification *hidNotification = (JoystickHIDNotification *)data;
            
            mapNotification((hidNotification->deviceId + 1) * JOYSTICK_OFFSET + hidNotification->usageId,
                            hidNotification->value);
        }
        else
        {
            OEUInt32 joystickNum = *((OEUInt32 *)data);
            
            if ((inputDevice == "Mouse") && joystickNum)
                inputDevice = "Joystick";
            else if ((inputDevice == "Joystick") && !joystickNum)
                inputDevice = "Mouse";
            
            update();
        }
    }
}

void JoystickMapper::addItem(string inputDevice, string label,
                             JoystickMapperType type, OEUInt32 usageId)
{
    JoystickMapperItem item;
    
    item.inputDevice = inputDevice;
    item.label = label;
    item.type = type;
    item.usageId = usageId;
    
    items.push_back(item);
}

void JoystickMapper::mapNotification(OEInt32 usageId, float value)
{
    for (JoystickMapperMap::iterator i = usageIdMap.begin();
         i != usageIdMap.end();
         i++)
    {
        if (i->second.usageId == usageId)
        {
            JoystickHIDNotification hidNotification;
            
            hidNotification.deviceId = deviceId;
            hidNotification.usageId = i->first;
            hidNotification.value = value;
            
            if (i->second.type == JOYSTICKMAPPER_AXIS)
            {
                hidNotification.value -= 0.5;
                if (i->second.reverse)
                    hidNotification.value = -hidNotification.value;
                hidNotification.value *= pow(10, i->second.sensitivity / 20);
                hidNotification.value += 0.5;
            }
            
            postNotification(this, JOYSTICK_DID_CHANGE, &hidNotification);
        }
    }
}

void JoystickMapper::setUsageId(OEInt32 usageId, string value)
{
    for (JoystickMapperItems::iterator i = items.begin();
         i != items.end();
         i++)
    {
        if (i->label == value)
        { 
            if (!usageIdMap.count(usageId))
                usageIdMap[usageId] = JoystickMapperRelation();
            
            usageIdMap[usageId].type = i->type;
            usageIdMap[usageId].usageId = i->usageId;
            
            return;
        }
    }
}

string JoystickMapper::getUsageId(OEInt32 usageId)
{
    OEUInt32 fromUsageId = usageIdMap[usageId].usageId;
    
    for (JoystickMapperItems::iterator i = items.begin();
         i != items.end();
         i++)
    {
        if (i->usageId == fromUsageId)
            return i->label;
    }
    
    return "";
}

void JoystickMapper::setSensitivity(OEInt32 usageId, float value)
{
    usageIdMap[usageId].sensitivity = value;
}

string JoystickMapper::getSensitivity(OEInt32 usageId)
{
    return getString(usageIdMap[usageId].sensitivity);
}

void JoystickMapper::setReverse(OEInt32 usageId, bool value)
{
    usageIdMap[usageId].reverse = value;
}

string JoystickMapper::getReverse(OEInt32 usageId)
{
    return getString(usageIdMap[usageId].reverse);
}

string JoystickMapper::serializeMap()
{
    vector<string> value;
    
    for (JoystickMapperMap::iterator i = usageIdMap.begin();
         i != usageIdMap.end();
         i++)
    {
        value.push_back(getString(i->first));
        value.push_back(getString(i->second.usageId));
        value.push_back(getString(i->second.sensitivity));
        value.push_back(getString(i->second.reverse));
    }
    
    return strjoin(value, ',');
}

void JoystickMapper::unserializeMap(string serialConf)
{
    vector<string> splitConf;
    
    splitConf = strsplit(serialConf, ',');
    
    int index = 0;
    for (int i = 0; i < (splitConf.size() / 4); i++)
    {
        OEUInt32 usageId = (OEUInt32) getInt(splitConf[index++]);
        
        usageIdMap[usageId].usageId = (OEUInt32) getInt(splitConf[index++]);
        usageIdMap[usageId].sensitivity = getFloat(splitConf[index++]);
        usageIdMap[usageId].reverse = (bool) getInt(splitConf[index++]);
    }
}
