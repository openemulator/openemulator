
/**
 * libemulator
 * Apple II Slot Expansion Memory
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot expansion memory ($C800-$CFFF)
 */

#include "OEComponent.h"

class AppleIISlotExpansionMemory : public OEComponent
{
public:
    AppleIISlotExpansionMemory();
    
	bool setRef(string name, OEComponent *ref);
	bool init();
    
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *mmu;
	OEComponent *memory;
};
