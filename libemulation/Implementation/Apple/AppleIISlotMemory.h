
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
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
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *floatingBus;
	OEComponent *slotExpansionMemory;
	
	OEComponent *slot[8];
	int slotSel;
};
