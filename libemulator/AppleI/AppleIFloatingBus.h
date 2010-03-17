
/**
 * libemulator
 * Apple I Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Floating Bus
 */

#include "OEComponent.h"

class AppleIFloatingBus : public OEComponent
{
public:
	int ioctl(int message, void * data);
	int read(int address);

private:
	OEComponent *system;
	OEComponent *memory;
};
