
/**
 * libemulation
 * Address masker
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address masker
 */

#include "OEComponent.h"

class AddressMasker : public OEComponent
{
public:
    AddressMasker();
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
private:
    OEComponent *memory;
    
    OEAddress andMask;
    OEAddress orMask;
};
