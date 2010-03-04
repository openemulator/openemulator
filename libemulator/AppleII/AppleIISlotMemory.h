
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C100-C7FF range.
 */

#include "OEComponent.h"

#define APPLEIISLOTMEMORY_SIZE 0x800

enum
{
	APPLEIISLOTMEMORY_QUERY = OEIOCTL_USER,
};

typedef struct
{
	OEComponent *slotIo;			// Get slot's io
	OEComponent *slotMemory;		// Get slot's memory
	OEComponent *expandedSlotMemory;// Get slot's expanded memory 
} AppleIISlotMemoryQuery;

class AppleIISlotMemory : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	int offset;
	
	OEComponent *system;
	OEComponent *mmu;
	OEComponent *floatingBus;
	OEComponent *io;
	OEComponent *expandedSlotMemory;

	OEComponent *slotMemoryMap[8];
	OEComponent *expandedSlotMemoryMap[8];
	
	void setSlot(int index, OEComponent *slotComponent);
};
