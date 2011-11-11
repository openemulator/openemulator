
/**
 * libemulator
 * Apple II Audio Output
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio output
 */

#include "OEComponent.h"

class AppleIIAudioOut : public OEComponent
{
public:
    AppleIIAudioOut();
    
	bool setRef(string name, OEComponent *ref);
	bool init();
    
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *floatingBus;
	OEComponent *audioCodec;
	
	OEUInt8 audioLevel;
    
    void toggleSpeaker();
};
