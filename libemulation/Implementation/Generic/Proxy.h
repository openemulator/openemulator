
/**
 * libemulation
 * Proxy
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a message proxy
 */

#include "OEComponent.h"

class Proxy : public OEComponent
{
public:
    Proxy();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    bool addObserver(OEComponent *observer, int notification);
    bool removeObserver(OEComponent *observer, int notification);
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    OEComponent *component;
};
