
/**
 * libemulation
 * Floating bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Manages a floating bus
 */

#include "OEComponent.h"

class FloatingBus : public OEComponent
{
public:
	FloatingBus();
	bool setProperty(const string &name, const string &value);
	
	OEUInt8 read(int address);
	
private:
	char busValue;
};
