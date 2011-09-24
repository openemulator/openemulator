
/**
 * libemulation
 * Monitor
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic monitor
 */

#include "OEComponent.h"

#include "CanvasInterface.h"
#include "ControlBusInterface.h"

class Monitor : public OEComponent
{
public:
    Monitor();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    OEComponent *device;
    OEComponent *controlBus;
    OEComponent *canvas;
    
    ControlBusPowerState powerState;
    
    CanvasDisplayConfiguration configuration;
};
