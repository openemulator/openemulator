
/**
 * libemulator
 * Apple I Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Floating Bus
 */

#include "OEComponent.h"

class AppleIFloatingBus : public OEComponent
{
public:
	bool connect(string name, OEComponent *component);
	
	int read(int address);

private:
	OEComponent *system;
	OEComponent *memory;
};
