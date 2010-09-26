
/**
 * libemulator
 * Apple II Game Port
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II's game port.
 */

#include "OEComponent.h"

class AppleIIGamePort : public OEComponent
{
public:
	bool setValue(string name, string value);
	bool setComponent(string name, OEComponent *component);
	
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	OEComponent *floatingBus;
	OEComponent *port;
	
	bool an0;
	bool an1;
	bool an2;
	bool an3;
};
