
/**
 * libemulation
 * Mux
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Multiplexes several components.
 */

#include "OEComponent.h"

#include "MemoryInterface.h"

class AddressMux : public OEComponent
{
public:
    AddressMux();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    OEShort read16(OEAddress address);
    void write16(OEAddress address, OEShort value);
    OEInt read32(OEAddress address);
    void write32(OEAddress address, OEInt value);
    OELong read64(OEAddress address);
    void write64(OEAddress address, OELong value);
    
private:
    MemoryMapsRef ref;
    
    string sel;
    
    OEComponent *component;
    
    OEComponent dummyComponent;
};
