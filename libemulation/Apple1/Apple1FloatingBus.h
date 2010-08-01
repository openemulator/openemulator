
/**
 * libemulation
 * Apple I Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Floating Bus
 */

#include "OEComponent.h"

class Apple1FloatingBus : public OEComponent
{
public:
	bool connect(const string &name, OEComponent *component);
	
	OEUInt8 read(int address);

private:
	OEComponent *system;
	OEComponent *memory;
};
