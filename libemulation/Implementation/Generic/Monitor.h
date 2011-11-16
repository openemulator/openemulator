
/**
 * libemulation
 * Monitor
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic monitor
 */

#include "OEComponent.h"

#include "ControlBusInterface.h"
#include "CanvasInterface.h"

class Monitor : public OEComponent
{
public:
    Monitor();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    void dispose();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    CanvasDisplayConfiguration configuration;
    
    OEComponent *device;
    OEComponent *controlBus;
    
    OEComponent *canvas;
    ControlBusPowerState powerState;
    
    void updateBezel();
};
