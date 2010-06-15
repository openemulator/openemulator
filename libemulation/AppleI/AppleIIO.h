
/**
 * libemulation
 * Apple I IO
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Input Output range
 */

#include "OEComponent.h"

class AppleIIO : public OEComponent
{
	bool setProperty(const string &name, const string &value);
	bool connect(const string &name, OEComponent *component);
	
	bool getMemoryMap(string &range);
	
	OEUInt8 read(int address);
	void write(int address, OEUInt8 value);
	
private:
	string mappedRange;
	
	OEComponent *pia;
	OEComponent *floatingBus;
};
