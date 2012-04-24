
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot memory ($C100-$C7FF)
 */

#include "OEComponent.h"

class AppleIISlotMemory : public OEComponent
{
public:
    AppleIISlotMemory();
    
	bool setValue(string name, string value);
    bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
    OEComponent *mmu;
    OEComponent *slotMemory;
	OEComponent *slotExpansionMemory;
	
	OEInt en;

    void mapMemory(int message);
};
