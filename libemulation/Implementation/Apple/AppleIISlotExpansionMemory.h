
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
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
	OEComponent *mmu;
	OEComponent *memory;
};
