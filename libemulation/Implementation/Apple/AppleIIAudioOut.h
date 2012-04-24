
/**
 * libemulator
 * Apple II Audio Output
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio output
 */

#include "Audio1Bit.h"

class AppleIIAudioOut : public Audio1Bit
{
public:
    AppleIIAudioOut();
    
	bool setRef(string name, OEComponent *ref);
	bool init();
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
	OEComponent *floatingBus;
};
