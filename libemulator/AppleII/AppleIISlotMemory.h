
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
	OEComponent *slotExpansionMemory;// Get slot's expanded memory 
} AppleIISlotMemoryQuery;

class AppleIISlotMemory : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	vector<string> mapVector;
	
	OEComponent *system;
	OEComponent *memoryMap;
	OEComponent *mmu;
	OEComponent *floatingBus;
	OEComponent *ioMap;
	OEComponent *slotExpansionMemory;
	
	OEComponent *slotMemoryMap[8];
	OEComponent *slotExpansionMemoryMap[8];
	
	void setSlot(int index, OEComponent *slotComponent);
};
