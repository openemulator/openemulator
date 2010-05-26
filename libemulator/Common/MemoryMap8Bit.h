
/**
 * libemulator
 * 8 bit memory map
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic 8-bit memory map
 */

#include "OEComponent.h"

#define MEMORYMAP8BIT_SIZE	0x100

class MemoryMap8Bit : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	OEMemoryRange mappedRange;
	
	OEComponent *readMap[MEMORYMAP8BIT_SIZE];
	OEComponent *writeMap[MEMORYMAP8BIT_SIZE];
	
	void setRange(OEComponent *component, vector<string> mapVector);
};
