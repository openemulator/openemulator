
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
    OEInt32 usageId;
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
    
    OEInt32 usageId;
    JoystickMapperType type;
    float sensitivity;
    bool reverse;
    float value;
};

typedef map<OEInt32,JoystickMapperRelation> JoystickMapperMap;

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
    
    OEInt32 deviceId;
    JoystickMapperMap usageIdMap;
    
    void addItem(string inputDevice, string label,
                 JoystickMapperType type, OEUInt32 usageId);
    OEInt32 getItemUsageId(string value);
    JoystickMapperType getItemType(string value);
    string getItemLabel(OEInt32 value);
    
    void mapNotification(OEInt32 usageId, float value);
    
    void setMap(OEInt32 usageId, string value);
    string getMap(OEInt32 usageId);
    void setSensitivity(OEInt32 usageId, float value);
    string getSensitivity(OEInt32 usageId);
    void setReverse(OEInt32 usageId, bool value);
    string getReverse(OEInt32 usageId);
    
    string serializeMap();
    void unserializeMap(string serialConf);
};
