
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
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	OEMemoryRange mappedRange;
	
	OEComponent *readMap[MEMORYMAP16BIT_SIZE];
	OEComponent *writeMap[MEMORYMAP16BIT_SIZE];
	
	void setRange(OEComponent *component, vector<string> mapVector);
};
