
/**
 * libemulator
 * Apple II Audio Input
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
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
    bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    void update();
    
	OEUInt8 read(OEAddress address);
	
private:
    float noiseRejection;
    
	OEComponent *floatingBus;
	OEComponent *audioCodec;
    
    OEUInt8 inputTriggerThreshold;
    OEUInt8 inputCurrentThreshold;
};
