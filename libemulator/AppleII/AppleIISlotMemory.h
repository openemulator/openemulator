
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C100-C7FF range.
 */

#include "OEComponent.h"

typedef struct 
{
	int index;
	OEComponent *slotMemory;
	OEComponent *expandedSlotMemory;
} SlotMemoryMessage;

class AppleIISlotMemory : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	OEComponent *expandedSlotMemory;
	 *slots[8];
	
	OEIoctlConnection connectionMessage;
};
