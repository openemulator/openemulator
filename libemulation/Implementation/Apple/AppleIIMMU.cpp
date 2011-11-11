
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
    
    slotIO.resize(APPLEIIMMU_SLOTIO);
    slotMemory.resize(APPLEIIMMU_SLOTMEMORY);
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
        slotIO[0] = ref;
	else if (name == "slot1IO")
        slotIO[1] = ref;
	else if (name == "slot2IO")
        slotIO[2] = ref;
	else if (name == "slot3IO")
        slotIO[3] = ref;
	else if (name == "slot4IO")
        slotIO[4] = ref;
	else if (name == "slot5IO")
        slotIO[5] = ref;
	else if (name == "slot6IO")
        slotIO[6] = ref;
	else if (name == "slot7IO")
        slotIO[7] = ref;
	else if (name == "slot1Memory")
        slotMemory[1] = ref;
	else if (name == "slot2Memory")
        slotMemory[2] = ref;
	else if (name == "slot3Memory")
        slotMemory[3] = ref;
	else if (name == "slot4Memory")
        slotMemory[4] = ref;
	else if (name == "slot5Memory")
        slotMemory[5] = ref;
	else if (name == "slot6Memory")
        slotMemory[6] = ref;
	else if (name == "slot7Memory")
        slotMemory[7] = ref;
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
    
    for (int i = 0; i < APPLEIIMMU_RAM; i++)
    {
        MemoryMap memoryMap;
        
        memoryMap.component = ram[i];
        memoryMap.startAddress = 0x0000 + 0x4000 * i;
        memoryMap.endAddress = 0x3fff + 0x4000 * i;
        memoryMap.read = true;
        memoryMap.write = true;
        
        memoryBus->postMessage(this, ADDRESSDECODER_MAP, &memoryMap);
    }
    
    for (int i = 0; i < APPLEIIMMU_ROM; i++)
    {
        MemoryMap memoryMap;
        
        memoryMap.component = rom[i];
        memoryMap.startAddress = 0xd000 + 0x0800 * i;
        memoryMap.endAddress = 0xd7ff + 0x0800 * i;
        memoryMap.read = true;
        memoryMap.write = false;
        
        memoryBus->postMessage(this, ADDRESSDECODER_MAP, &memoryMap);
    }
    
    return true;
}

void AppleIIMMU::dispose()
{
    
}

bool AppleIIMMU::postMessage(OEComponent *sender, int message, void *data)
{
    switch(message)
    {
    }
    
    return false;
}
