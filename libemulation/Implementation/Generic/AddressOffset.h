
/**
 * libemulation
 * Address offset
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address offset
 */

#include "OEComponent.h"

class AddressOffset : public OEComponent
{
public:
    AddressOffset();
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEAddress offset;
    
    OEComponent *component;
};
