
/**
 * libemulator
 * Apple II Audio Output
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio output
 */

#include "OEComponent.h"

class AppleIIAudioOut : public OEComponent
{
public:
	bool setValue(string name, string value);
	bool setRef(string name, OEComponent *ref);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *sampleConverter;
	OEComponent *floatingBus;
	
	bool state;
};
