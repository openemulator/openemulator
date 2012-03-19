
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
    void update();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
    OEUInt16 read16(OEAddress address);
    void write16(OEAddress address, OEUInt16 value);
    OEUInt32 read32(OEAddress address);
    void write32(OEAddress address, OEUInt32 value);
    OEUInt64 read64(OEAddress address);
    void write64(OEAddress address, OEUInt64 value);
    
private:
    MemoryMapsRef ref;
    
    string sel;
    
    OEComponent *component;
    
    OEComponent dummyComponent;
};
