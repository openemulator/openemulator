
/**
 * libemulator
 * 16 bit memory map
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic 16-bit memory map
 */

#define MEMORYMAP16BIT_SIZE		0x100
#define MEMORYMAP16BIT_SHIFT	8

#include "OEComponent.h"

class MemoryMap16Bit : public OEComponent
{
public:
	bool setProperty(string name, string value);
	bool connect(string name, OEComponent *component);
	
	bool setMemoryMap(OEComponent *component, OEMemoryRange range);
	bool getMemoryMap(OEMemoryRange &range);
	
	int read(int address);
	void write(int address, int value);
	
private:
	OEMemoryRange mappedRange;
	
	OEComponent *readMap[MEMORYMAP16BIT_SIZE];
	OEComponent *writeMap[MEMORYMAP16BIT_SIZE];
};
