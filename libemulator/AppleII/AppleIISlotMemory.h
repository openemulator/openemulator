
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
	OEComponent *slotIo;			// Get slot's io
	OEComponent *slotMemory;		// Get slot's memory
	OEComponent *expandedSlotMemory;// Get slot's expanded memory 
} SlotMemoryQuery;

class AppleIISlotMemory : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	OEComponent *floatingBus;
	OEComponent *expandedSlotMemory;
	
	OEComponent *slotMemoryMap[8];
	OEComponent *expandedSlotMemoryMap[8];
	
	OEIoctlConnection connectionMessage;
	
	void setSlot(int index, OEComponent *slotComponent);
};
