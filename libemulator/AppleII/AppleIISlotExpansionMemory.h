
/**
 * libemulator
 * Apple II Slot Expansion Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C800-CFFF range.
 */

#include "OEComponent.h"

enum
{
	APPLEIISLOTEXPANSIONMEMORY_SET_SLOT = OE_USER,
};

#define APPLEIISLOTEXPANSIONMEMORY_SIZE	0x800
#define APPLEIISLOTEXPANSIONMEMORY_MASK 0x7ff

class AppleIISlotExpansionMemory : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	OEMemoryRange mappedRange;
	
	OEComponent *floatingBus;
	OEComponent *expandedSlotMemory;
};
