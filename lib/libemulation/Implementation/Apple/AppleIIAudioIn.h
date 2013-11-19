
/**
 * libemulator
 * Apple II Audio Input
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio input
 */

#include "Audio1Bit.h"

class AppleIIAudioIn : public Audio1Bit
{
public:
    AppleIIAudioIn();
    
	bool setRef(string name, OEComponent *ref);
	bool init();
    
	OEChar read(OEAddress address);
	
private:
	OEComponent *floatingBus;
};
