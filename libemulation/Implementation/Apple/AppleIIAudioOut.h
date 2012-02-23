
/**
 * libemulator
 * Apple II Audio Output
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio output
 */

#include "OEComponent.h"

class AppleIIAudioOut : public OEComponent
{
public:
    AppleIIAudioOut();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    void update();
    
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *floatingBus;
	OEComponent *audioCodec;
	
    float volume;
    
    bool outputState;
	OEInt16 outputHighLevel;
    
    void toggleSpeaker();
};
