
/**
 * libemulation
 * Apple I Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Video
 */

#include "AppleIVideo.h"

AppleIVideo::AppleIVideo()
{
	memset(screen, 0, APPLEIVIDEO_TERM_HEIGHT * APPLEIVIDEO_TERM_HEIGHT);
}

bool AppleIVideo::setResource(const string &name, const OEData *data)
{
	if (name == "image")
		characterSet = *data;
	else
		return false;
	
	return true;
}

bool AppleIVideo::connect(const string &name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else if (name == "system")
		system = component;
	else if (name == "pia")
		pia = component;
	else if (name == "monitor")
		monitor = component;
	else if (name == "videoROM")
		videoROM = component;
	else
		return false;
	
	return true;
}

void AppleIVideo::write(int address, int value)
{
}
