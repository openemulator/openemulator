
/**
 * libemulator
 * Apple II Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Provides the Apple II floating bus
 */

#include "OEComponent.h"

class AppleIIFloatingBus : public OEComponent
{
public:
	int ioctl(int message, void *data);
	int read(int address);
	
private:
	int offset;
	int size;
};
