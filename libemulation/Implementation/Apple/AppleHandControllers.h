
/**
 * libemulation
 * Apple Hand Controllers
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple Hand Controllers
 */

#include "OEComponent.h"

typedef struct
{
    OEInt32 pdl[4];
    bool reversePDL[4];
    OEInt32 pb[3];
} AppleHandControllersConf;

class AppleHandControllers : public OEComponent
{
public:
    AppleHandControllers();
    
    bool setValue(string name, string value);
    bool getValue(string name, string &value);
    bool setRef(string name, OEComponent *ref);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    OEComponent *gamePort;
    OEComponent *video;
    OEComponent *joystick;
    
    map<string, OEUInt32> nameMap;
    map<string, AppleHandControllersConf> presets;
    
    AppleHandControllersConf conf;
    
    void setPreset(string name);
    string getPreset();
    void setPDL(int index, string value);
    string getPDL(int index);
    void setPB(int index, string value);
    string getPB(int index);
};
