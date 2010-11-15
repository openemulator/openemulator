
/**
 * libemulator
 * Apple II Memory Management Unit
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II D000-FFFF range.
 */

#include "OEComponent.h"

class AppleIIMMU : public OEComponent
{
public:
	AppleIIMMU();
	
	bool setRef(string name, OEComponent *ref);	
	
private:
	OEComponent *memoryBus;
	OEComponent *floatingBus;
	
	OEComponent *romD0;
	OEComponent *romD8;
	OEComponent *romE0;
	OEComponent *romE8;
	OEComponent *romF0;
	OEComponent *romF8;
	
	OEComponent *romD0Socket;
	OEComponent *romD8Socket;
	OEComponent *romE0Socket;
	OEComponent *romE8Socket;
	OEComponent *romF0Socket;
	OEComponent *romF8Socket;
};
