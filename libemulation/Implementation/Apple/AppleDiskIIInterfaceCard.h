
/**
 * libemulation
 * Apple Disk II Interface Card
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple Disk II interface card
 */

#include "OEComponent.h"

class AppleDiskIIInterfaceCard : public OEComponent
{
public:
	AppleDiskIIInterfaceCard();
	
	bool setValue(string name, string value);
	bool getValue(string name, string &value);
	bool setRef(string name, OEComponent *ref);
	
private:
	OEComponent *controlBus;
	
	bool controllerType;
};
