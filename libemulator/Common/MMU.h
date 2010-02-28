
/**
 * libemulator
 * MMU
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic memory management unit
 */

#define MMU_MAPSIZE		0x10000
#define MMU_PAGESIZE	0x100
#define MMU_PAGESHIFT	8

#include "OEComponent.h"

class MMU : public OEComponent
{
public:
	MMU();
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);
	
private:
	int offset;
	OEComponent *floatingBus;
	OEComponent *readMap[MMU_MAPSIZE];
	OEComponent *writeMap[MMU_MAPSIZE];
	
	void setFloatingBus(OEComponent *component);
	void setRange(OEComponent *component, int start, int end);
};
