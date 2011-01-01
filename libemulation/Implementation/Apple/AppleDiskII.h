
/**
 * libemulation
 * Apple Disk II
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Disk II drive
 */

#include "Terminal.h"

class AppleDiskII : public Terminal
{
public:
	bool setValue(string name, string value);
	bool getValue(string name, string &value);
	bool setRef(string name, OEComponent *ref);
	
private:
	OEComponent *emulation;
	
	OEComponent *storage;
	
	bool forceWriteProtected;
};
