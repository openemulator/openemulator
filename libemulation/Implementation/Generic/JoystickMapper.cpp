
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

#define MOUSE_START 0x100
#define JOYSTICK_START 0x200
#define JOYSTICK_OFFSET 0x100

JoystickMapper::JoystickMapper()
{
    device = NULL;
    canvas = NULL;
    joystick = NULL;
    
    for (OEInt i = 0; i < JOYSTICK_AXIS_NUM; i++)
        addItem("", "axis" + getString(i), JOYSTICKMAPPER_AXIS, JOYSTICK_AXIS1 + i);
    for (OEInt i = 0; i < JOYSTICK_BUTTON_NUM; i++)
        addItem("", "button" + getString(i), JOYSTICKMAPPER_BUTTON, JOYSTICK_BUTTON1 + i);
    
    addItem("Mouse", "Mouse X", JOYSTICKMAPPER_RELATIVEAXIS, MOUSE_START + CANVAS_M_RX);
    addItem("Mouse", "Mouse Y", JOYSTICKMAPPER_RELATIVEAXIS, MOUSE_START + CANVAS_M_RY);
    addItem("Mouse", "Mouse Wheel X", JOYSTICKMAPPER_RELATIVEAXIS, MOUSE_START + CANVAS_M_WHEELX);
    addItem("Mouse", "Mouse Wheel Y", JOYSTICKMAPPER_RELATIVEAXIS, MOUSE_START + CANVAS_M_WHEELY);
    for (OEInt i = 0; i < CANVAS_MOUSE_BUTTON_NUM; i++)
        addItem("Mouse", "Mouse Button " + getString(i + 1), JOYSTICKMAPPER_BUTTON,
                MOUSE_START + CANVAS_M_BUTTON1 + i);
    addItem("Mouse", "Unmapped", JOYSTICKMAPPER_UNMAPPED, -1);
    
    for (OEInt deviceId = 0; deviceId < 4; deviceId++)
    {
        string deviceLabel = "Joystick " + getString(deviceId + 1);
        OEInt deviceOffset = JOYSTICK_START + JOYSTICK_OFFSET * deviceId;
        
        for (OEInt i = 0; i < JOYSTICK_AXIS_NUM; i++)
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
        for (OEInt i = 0; i < JOYSTICK_BUTTON_NUM; i++)
            addItem("Joystick", deviceLabel + " Button " + getString(i + 1), JOYSTICKMAPPER_BUTTON,
                    deviceOffset + JOYSTICK_BUTTON1 + i);
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
        deviceId = getOEInt(value);
    else if (name.substr(0, 4) == "axis")
        setMap(JOYSTICK_AXIS1 + getOEInt(name.substr(4)), value);
    else if (name.substr(0, 6) == "button")
        setMap(JOYSTICK_BUTTON1 + getOEInt(name.substr(6)), value);
    else if (name.substr(0, 15) == "sensitivityAxis")
        setSensitivity(JOYSTICK_AXIS1 + getOEInt(name.substr(15)), getFloat(value));
    else if (name.substr(0, 11) == "reverseAxis")
        setReverse(JOYSTICK_AXIS1 + getOEInt(name.substr(11)), getOEInt(value));
    else if (name.substr(0, 3) == "map")
        inputDeviceMap[name.substr(3)] = value;
    else
        return false;
    
    return true;
}

bool JoystickMapper::getValue(string name, string& value)
{
    if (name == "inputDevice")
        value = inputDevice;
    else if (name == "deviceId")
        value = getString(deviceId);
    else if (name.substr(0, 4) == "axis")
        value = getMap(JOYSTICK_AXIS1 + getOEInt(name.substr(4)));
    else if (name.substr(0, 6) == "button")
        value = getMap(JOYSTICK_BUTTON1 + getOEInt(name.substr(6)));
    else if (name.substr(0, 15) == "sensitivityAxis")
        value = getSensitivity(JOYSTICK_AXIS1 + getOEInt(name.substr(15)));
    else if (name.substr(0, 11) == "reverseAxis")
        value = getReverse(JOYSTICK_AXIS1 + getOEInt(name.substr(11)));
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
            joystick->removeObserver(this, JOYSTICK_WAS_CONNECTED);
            joystick->removeObserver(this, JOYSTICK_WAS_DISCONNECTED);
        }
        joystick = ref;
        if (joystick)
        {
            joystick->addObserver(this, JOYSTICK_DID_CHANGE);
            joystick->addObserver(this, JOYSTICK_WAS_CONNECTED);
            joystick->addObserver(this, JOYSTICK_WAS_DISCONNECTED);
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
    
    update();
    
    return true;
}

void JoystickMapper::update()
{
    if (inputDevice != oldInputDevice)
    {
        // Build axis and button label
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
        
        // Update settings
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
        
        // Load map
        unserializeMap(inputDeviceMap[inputDevice]);
        
        // Reset values
        for (JoystickMapperMap::iterator i = usageIdMap.begin();
             i != usageIdMap.end();
             i++)
        {
            if ((i->second.type == JOYSTICKMAPPER_BUTTON) &&
                (i->second.value))
            {
                i->second.value = false;
                
                JoystickHIDEvent hidEvent;
                
                hidEvent.deviceId = deviceId;
                hidEvent.usageId = i->first;
                hidEvent.value = i->second.value;
                
                postNotification(this, JOYSTICK_DID_CHANGE, &hidEvent);
            }
        }
        
        oldInputDevice = inputDevice;
    }
}

void JoystickMapper::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == canvas)
    {
        CanvasHIDEvent *hidEvent = (CanvasHIDEvent *)data;
        
        mapNotification(MOUSE_START + hidEvent->usageId, hidEvent->value);
    }
    else if (sender == joystick)
    {
        if (notification == JOYSTICK_DID_CHANGE)
        {
            JoystickHIDEvent *hidEvent = (JoystickHIDEvent *)data;
            
            mapNotification(JOYSTICK_START + JOYSTICK_OFFSET * hidEvent->deviceId + hidEvent->usageId,
                            hidEvent->value);
        }
        else
        {
            OEInt joystickNum = *((OEInt *)data);
            
            if ((inputDevice == "Mouse") && joystickNum)
                inputDevice = "Joystick";
            else if ((inputDevice == "Joystick") && !joystickNum)
                inputDevice = "Mouse";
            
            update();
        }
    }
}

void JoystickMapper::addItem(string inputDevice, string label,
                             JoystickMapperType type, OEInt usageId)
{
    JoystickMapperItem item;
    
    item.inputDevice = inputDevice;
    item.label = label;
    item.type = type;
    item.usageId = usageId;
    
    items.push_back(item);
}

OEInt JoystickMapper::getItemUsageId(string label)
{
    for (JoystickMapperItems::iterator i = items.begin();
         i != items.end();
         i++)
    {
        if (i->label == label)
            return i->usageId;
    }
    
    return -1;
}

JoystickMapperType JoystickMapper::getItemType(string label)
{
    for (JoystickMapperItems::iterator i = items.begin();
         i != items.end();
         i++)
    {
        if (i->label == label)
            return i->type;
    }
    
    return JOYSTICKMAPPER_UNMAPPED;
}

string JoystickMapper::getItemLabel(OEInt usageId)
{
    for (JoystickMapperItems::iterator i = items.begin();
         i != items.end();
         i++)
    {
        if (i->usageId == usageId)
            return i->label;
    }
    
    return "";
}

void JoystickMapper::mapNotification(OEInt usageId, float value)
{
    for (JoystickMapperMap::iterator i = usageIdMap.begin();
         i != usageIdMap.end();
         i++)
    {
        if (i->second.usageId == usageId)
        {
            JoystickHIDEvent hidEvent;
            
            hidEvent.deviceId = deviceId;
            hidEvent.usageId = i->first;
            hidEvent.value = value;
            
            switch (i->second.type)
            {
                case JOYSTICKMAPPER_AXIS:
                    hidEvent.value -= 0.5;
                    if (i->second.reverse)
                        hidEvent.value = -hidEvent.value;
                    hidEvent.value *= pow(10, i->second.sensitivity / 20);
                    hidEvent.value += 0.5;
                    
                    break;
                    
                case JOYSTICKMAPPER_RELATIVEAXIS:
                    if (i->second.reverse)
                        hidEvent.value = -hidEvent.value;
                    hidEvent.value = i->second.value + hidEvent.value * pow(10, (i->second.sensitivity - 50) / 20);
                    
                    break;
                    
                default:
                    break;
            }
            
            if (hidEvent.value < 0)
                hidEvent.value = 0;
            else if (hidEvent.value > 1)
                hidEvent.value = 1;
            
            if (i->second.value == hidEvent.value)
                return;
            
            i->second.value = hidEvent.value;
            
            postNotification(this, JOYSTICK_DID_CHANGE, &hidEvent);
        }
    }
}

void JoystickMapper::setMap(OEInt usageId, string value)
{
    usageIdMap[usageId].type = getItemType(value);
    usageIdMap[usageId].usageId = getItemUsageId(value);
    usageIdMap[usageId].value = 0.5;
}

string JoystickMapper::getMap(OEInt usageId)
{
    return getItemLabel(usageIdMap[usageId].usageId);
}

void JoystickMapper::setSensitivity(OEInt usageId, float value)
{
    usageIdMap[usageId].sensitivity = value;
}

string JoystickMapper::getSensitivity(OEInt usageId)
{
    return getString(usageIdMap[usageId].sensitivity);
}

void JoystickMapper::setReverse(OEInt usageId, bool value)
{
    usageIdMap[usageId].reverse = value;
}

string JoystickMapper::getReverse(OEInt usageId)
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
        JoystickMapperItem item;
        
        value.push_back(getItemLabel(i->first));
        value.push_back(getItemLabel(i->second.usageId));
        value.push_back(getString(i->second.sensitivity));
        value.push_back(getString(i->second.reverse));
    }
    
    return strjoin(value, ',');
}

void JoystickMapper::unserializeMap(string serialConf)
{
    vector<string> splitConf;
    
    splitConf = strsplit(serialConf, ',');
    
    OEInt index = 0;
    for (OEInt i = 0; i < (splitConf.size() / 4); i++)
    {
        JoystickMapperItem item;
        
        OEInt usageId = getItemUsageId(splitConf[index++]);
        
        string usageName = splitConf[index++];
        float sensitivity = getFloat(splitConf[index++]);
        bool reverse = getOEInt(splitConf[index++]);
        
        usageIdMap[usageId].usageId = getItemUsageId(usageName);
        usageIdMap[usageId].type = getItemType(usageName);
        usageIdMap[usageId].sensitivity = sensitivity;
        usageIdMap[usageId].reverse = reverse;
        usageIdMap[usageId].value = 0.5;
    }
}
