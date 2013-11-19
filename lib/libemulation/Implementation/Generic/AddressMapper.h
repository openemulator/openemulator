
/**
 * libemulation
 * Address mapper
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Maps components in an address decoder.
 */

#include "OEComponent.h"

#include "MemoryInterface.h"

class AddressMapper : public OEComponent
{
public:
    AddressMapper();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    void dispose();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
    OEComponent *addressDecoder;
    
    MemoryMapsConf conf;
    MemoryMapsRef ref;
    
    string lastSel;
    string sel;
    
    MemoryMaps buildMemoryMaps(string value);
};
