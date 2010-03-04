
/**
 * libemulator
 * Apple II Expanded Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C800-CFFF range.
 */

#include "OEComponent.h"

class AppleIIExpansionSlotMemory : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	void write(int address, int value);

private:
	OEComponent *floatingBus;
	OEComponent *expandedSlotMemory;
};
