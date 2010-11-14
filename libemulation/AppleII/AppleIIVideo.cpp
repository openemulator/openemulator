
/**
 * libemulator
 * Apple II Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II video.
 */

#include "AppleIIVideo.h"

bool AppleIIVideo::setRef(string name, OEComponent *id)
{
	if (name == "host")
		host = id;
	else if (name == "ram")
		ram = id;
	else if (name == "charset")
		charset = id;
	else if (name == "charsetSocket")
		charsetSocket = id;
	else if (name == "monitor")
		monitor = id;
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
