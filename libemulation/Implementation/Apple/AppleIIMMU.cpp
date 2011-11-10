
/**
 * libemulator
 * Apple II Memory Management Unit
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II memory ($D000-$FFFF)
 */

#include "AppleIIMMU.h"

#include "MemoryInterface.h"
#include "AppleIIInterface.h"

AppleIIMMU::AppleIIMMU()
{
	memoryBus = NULL;
	floatingBus = NULL;
	
    for (int i = 0; i < 3; i++)
        ram[i] = NULL;
    for (int i = 0; i < 6; i++)
        rom[i] = NULL;
    for (int i = 0; i < 8; i++)
        slotIO[i] = NULL;
    for (int i = 0; i < 8; i++)
        slotMemory[i] = NULL;
}

bool AppleIIMMU::setRef(string name, OEComponent *ref)
{
	if (name == "memoryBus")
		memoryBus = ref;
	else if (name == "floatingBus")
		floatingBus = ref;
	else if (name == "ram1")
		ram[0] = ref;
	else if (name == "ram2")
		ram[1] = ref;
	else if (name == "ram3")
		ram[2] = ref;
	else if (name == "romD0")
		rom[0] = ref;
	else if (name == "romD8")
		rom[1] = ref;
	else if (name == "romE0")
		rom[2] = ref;
	else if (name == "romE8")
		rom[3] = ref;
	else if (name == "romF0")
		rom[4] = ref;
	else if (name == "romF8")
		rom[5] = ref;
	else if (name == "videoSync")
		videoSync = ref;
	else if (name == "slot0IO")
        updateSlotIO(0, ref);
	else if (name == "slot1IO")
        updateSlotIO(1, ref);
	else if (name == "slot2IO")
        updateSlotIO(2, ref);
	else if (name == "slot3IO")
        updateSlotIO(3, ref);
	else if (name == "slot4IO")
        updateSlotIO(4, ref);
	else if (name == "slot5IO")
        updateSlotIO(5, ref);
	else if (name == "slot6IO")
        updateSlotIO(6, ref);
	else if (name == "slot7IO")
        updateSlotIO(7, ref);
	else if (name == "slot1Memory")
        updateSlotMemory(1, ref);
	else if (name == "slot2Memory")
        updateSlotMemory(2, ref);
	else if (name == "slot3Memory")
        updateSlotMemory(3, ref);
	else if (name == "slot4Memory")
        updateSlotMemory(4, ref);
	else if (name == "slot5Memory")
        updateSlotMemory(5, ref);
	else if (name == "slot6Memory")
        updateSlotMemory(6, ref);
	else if (name == "slot7Memory")
        updateSlotMemory(7, ref);
	else
		return false;
	
	return true;
}

bool AppleIIMMU::init()
{
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    return true;
}

bool AppleIIMMU::postMessage(OEComponent *sender, int message, void *data)
{
    switch(message)
    {
    }
    
    return false;
}

void AppleIIMMU::updateSlotIO(int index, OEComponent *ref)
{
    
}

void AppleIIMMU::updateSlotMemory(int index, OEComponent *ref)
{
    
}
