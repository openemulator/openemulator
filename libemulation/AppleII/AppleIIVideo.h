
/**
 * libemulator
 * Apple II Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II video.
 */

#include "OEComponent.h"

class AppleIIVideo : public OEComponent
{
public:
	bool setComponent(string name, OEComponent *component);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *host;
	OEComponent *ram;
	OEComponent *charset;
	OEComponent *charsetSocket;
	OEComponent *monitor;
};
