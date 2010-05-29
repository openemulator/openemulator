
/**
 * libemulator
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

bool AppleIVideo::setResource(const string &name, const OEData &data)
{
	if (name == "image")
		characterSet = data;
	else
		return false;
	
	return true;
}

bool AppleIVideo::connect(const string &name, OEComponent *component)
{
	if (name == "system")
//		component->addObserver(this, <#int notification#>)
		;
	else
		return false;
	
	return true;
}

void AppleIVideo::write(int address, int value)
{
	
	
}
