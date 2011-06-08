
/**
 * libemulator
 * Apple II Slot Expansion Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot expansion memory ($C800-$CFFF)
 */

#include "OEComponent.h"

typedef enum
{
	APPLEIISLOTEXPANSIONMEMORY_SET_SLOT,
} AppleIISlotExpansionMemoryMessage;

class AppleIISlotExpansionMemory : public OEComponent
{
public:
	bool setRef(string name, OEComponent *ref);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *floatingBus;
	OEComponent *slot;
};
