
/**
 * libemulator
 * Apple II Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II floating bus.
 */

#include "OEComponent.h"

class AppleIIFloatingBus : public OEComponent
{
public:
	bool setRef(string name, OEComponent *id);
	
	OEUInt8 read(OEAddress address);
	
private:
	OEComponent *controlBus;
	OEComponent *ram;
};
