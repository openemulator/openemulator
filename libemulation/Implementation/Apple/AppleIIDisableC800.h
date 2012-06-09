
/**
 * libemulator
 * Apple II Disable C800
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Disables Apple II slot expansion memory ($C800-$CFFF)
 */

#include "OEComponent.h"

class AppleIIDisableC800 : public OEComponent
{
public:
    AppleIIDisableC800();
    
	bool setRef(string name, OEComponent *ref);
	bool init();
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
    OEComponent *memory;
    OEComponent *memoryBus;
};
