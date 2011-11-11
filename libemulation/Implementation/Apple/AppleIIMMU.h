
/**
 * libemulator
 * Apple II Memory Management Unit
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II memory ($D000-$FFFF)
 */

#include "OEComponent.h"

#define APPLEIIMMU_RAM 3
#define APPLEIIMMU_ROM 6
#define APPLEIIMMU_SLOTIO 8
#define APPLEIIMMU_SLOTMEMORY 8

class AppleIIMMU : public OEComponent
{
public:
	AppleIIMMU();
	
	bool setRef(string name, OEComponent *ref);	
	bool init();
    void dispose();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
	OEComponent *memoryBus;
	OEComponent *floatingBus;
	
    OEComponent *ram[APPLEIIMMU_RAM];
	OEComponent *rom[APPLEIIMMU_ROM];
    OEComponent *videoSync;
    OEComponents slotIO;
    OEComponents slotMemory;
};
