
/**
 * libemulation
 * Apple Disk II
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Disk II drive
 */

#include "AppleDiskII.h"

#include "OEEmulation.h"

bool AppleDiskII::setRef(string name, OEComponent *ref)
{
	if (name == "emulation")
	{
		if (emulation)
			emulation->postMessage(this,
								   OEEMULATION_REMOVE_STORAGE,
								   &storage);
		emulation = ref;
		if (emulation)
			emulation->postMessage(this,
								   OEEMULATION_ADD_STORAGE,
								   &storage);
	}
	else
		return false;
	
	return true;
}
