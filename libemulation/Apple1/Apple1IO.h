
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
	
	bool setRef(string name, OEComponent *ref);
	void notify(OEComponent *sender, int notification, void *data);
	
	OEUInt8 read(OEAddress address);
	
private:
	OEComponent *terminal;
	
	int key;
};
