
/**
 * libemulator
 * Apple II Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II video.
 */

#include "AppleIIVideo.h"

bool AppleIIVideo::setRef(string name, OEComponent *ref)
{
	if (name == "host")
		host = ref;
	else if (name == "ram")
		ram = ref;
	else if (name == "charset")
		charset = ref;
	else if (name == "charsetSocket")
		charsetSocket = ref;
	else if (name == "monitor")
		monitor = ref;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIIVideo::read(OEAddress address)
{
	return 0;
}

void AppleIIVideo::write(OEAddress address, OEUInt8 value)
{
}
