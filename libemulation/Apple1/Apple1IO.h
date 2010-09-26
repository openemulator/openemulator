
/**
 * libemulation
 * Apple I input/output
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple I input/output
 */

#include "OEComponent.h"

class Apple1IO : public OEComponent
{
public:
	Apple1IO();
	
	bool setComponent(string name, OEComponent *component);
	void notify(int notification, OEComponent *component, void *data);
	
	OEUInt8 read(int address);
	
private:
	OEComponent *terminal;
	
	int key;
};
