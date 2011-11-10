
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot memory ($C100-$C7FF)
 */

#include "OEComponent.h"

class AppleIISlotMemory : public OEComponent
{
public:
	bool setValue(string name, string value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *floatingBus;
	OEComponent *slotExpansionMemory;
	
	OEComponent *slot[8];
	OEUInt32 slotSel;
    
    void setSlot(OEUInt32 slotIndex, OEComponent *ref);
};
