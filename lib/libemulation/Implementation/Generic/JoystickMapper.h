
/**
 * libemulation
 * Joystick Mapper
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a Joystick Mapper
 */

#include "OEComponent.h"

enum JoystickMapperType
{
    JOYSTICKMAPPER_AXIS,
    JOYSTICKMAPPER_RELATIVEAXIS,
    JOYSTICKMAPPER_BUTTON,
    JOYSTICKMAPPER_UNMAPPED,
};

class JoystickMapperItem
{
public:
    JoystickMapperItem()
    {
        usageId = 0;
        type = JOYSTICKMAPPER_UNMAPPED;
    }
    
    string inputDevice;
    string label;
    OEInt usageId;
    JoystickMapperType type;
};

typedef vector<JoystickMapperItem> JoystickMapperItems;

class JoystickMapperRelation
{
public:
    JoystickMapperRelation()
    {
        usageId = 0;
        type = JOYSTICKMAPPER_UNMAPPED;
        sensitivity = 0;
        reverse = false;
        value = 0;
    }
    
    OEInt usageId;
    JoystickMapperType type;
    float sensitivity;
    bool reverse;
    float value;
};

typedef map<OEInt,JoystickMapperRelation> JoystickMapperMap;

class JoystickMapper : public OEComponent
{
public:
    JoystickMapper();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    OEComponent *device;
    OEComponent *canvas;
    OEComponent *joystick;
    
    JoystickMapperItems items;
    
    string inputDevice;
    string oldInputDevice;
    map<string, string> inputDeviceMap;
    
    OEInt deviceId;
    JoystickMapperMap usageIdMap;
    
    void addItem(string inputDevice, string label,
                 JoystickMapperType type, OEInt usageId);
    OEInt getItemUsageId(string value);
    JoystickMapperType getItemType(string value);
    string getItemLabel(OEInt value);
    
    void mapNotification(OEInt usageId, float value);
    
    void setMap(OEInt usageId, string value);
    string getMap(OEInt usageId);
    void setSensitivity(OEInt usageId, float value);
    string getSensitivity(OEInt usageId);
    void setReverse(OEInt usageId, bool value);
    string getReverse(OEInt usageId);
    
    string serializeMap();
    void unserializeMap(string serialConf);
};
