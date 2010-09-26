
/**
 * libemulator
 * Apple II Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II video.
 */

#include "AppleIIVideo.h"

bool AppleIIVideo::setComponent(string name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else if (name == "ram")
		ram = component;
	else if (name == "charset")
		charset = component;
	else if (name == "charsetSocket")
		charsetSocket = component;
	else if (name == "monitor")
		monitor = component;
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
