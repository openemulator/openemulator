
/**
 * libemulator
 * Apple II Floating Bus
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the Apple II floating bus
 */

#include "OEComponent.h"

class AppleIIFloatingBus : public OEComponent
{
public:
    AppleIIFloatingBus();
    
	bool setRef(string name, OEComponent *ref);
    bool init();
	
	OEUInt8 read(OEAddress address);
	
private:
	OEComponent *video;
};
