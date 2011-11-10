
/**
 * libemulator
 * Apple II Memory Management Unit
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II memory ($D000-$FFFF)
 */

#include "OEComponent.h"

class AppleIIMMU : public OEComponent
{
public:
	AppleIIMMU();
	
	bool setRef(string name, OEComponent *ref);	
	bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
	OEComponent *memoryBus;
	OEComponent *floatingBus;
	
    OEComponent *ram[3];
	OEComponent *rom[6];
    OEComponent *videoSync;
    OEComponent *slotIO[8];
    OEComponent *slotMemory[8];
    
    void updateSlotIO(int index, OEComponent *ref);
    void updateSlotMemory(int index, OEComponent *ref);
};
