
/**
 * libemulator
 * Apple II Audio Input
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio input.
 */

#include "OEComponent.h"

class AppleIIAudioIn : public OEComponent
{
public:
	bool setRef(string name, OEComponent *ref);
	
	OEUInt8 read(OEAddress address);
	
private:
	OEComponent *sampleConverter;
	OEComponent *floatingBus;
};
