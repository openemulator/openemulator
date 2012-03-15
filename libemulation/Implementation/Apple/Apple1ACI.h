
/**
 * libemulation
 * Apple-1 ACI
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple-1 ACI (Apple Cassette Interface)
 */

#include "Audio1Bit.h"

class Apple1ACI : public Audio1Bit
{
public:
    Apple1ACI();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    void dispose();
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
    
private:
    OEComponent *memoryBus;
    OEComponent *rom;
    
    void mapMemory(int message);
};
