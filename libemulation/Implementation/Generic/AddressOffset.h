
/**
 * libemulation
 * Address offset
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address offseter
 */

#include "OEComponent.h"

#include "MemoryInterface.h"

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
    OEComponent *memory;
    
    OEAddress size;
    OEAddress blockSize;
    
    OEInt blockBits;
    
    vector<OESLong> offset;
    OESLong *offsetp;
    
    OEAddress mask;
    
    AddressOffsetMaps offsetMaps;
    
    bool mapOffset(AddressOffsetMap& value);
};
