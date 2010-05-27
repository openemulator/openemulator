
/**
 * libemulator
 * Apple I IO
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Input Output range
 */

#include "OEComponent.h"

class AppleIIO : public OEComponent
{
	bool setProperty(string name, string value);
	bool connect(string name, OEComponent *component);
	
	bool getMemoryMap(string &range);
	
	int read(int address);
	void write(int address, int value);
	
private:
	string mappedRange;
	
	OEComponent *pia;
	OEComponent *floatingBus;
};
