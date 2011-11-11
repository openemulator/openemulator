
/**
 * libemulator
 * Apple II Audio Input
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio input
 */

#include "OEComponent.h"

class AppleIIAudioIn : public OEComponent
{
public:
    AppleIIAudioIn();
    
    bool setValue(string name, string value);
    bool getValue(string name, string &value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    
	OEUInt8 read(OEAddress address);
	
private:
	OEComponent *floatingBus;
	OEComponent *audioCodec;
    
    OEUInt8 noiseRejection;
    OEUInt8 threshold;
};
