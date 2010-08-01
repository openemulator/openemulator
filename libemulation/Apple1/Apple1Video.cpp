
/**
 * libemulation
 * Apple I Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Video
 */

#include "Apple1Video.h"

Apple1Video::Apple1Video()
{
	memset(screen, 0, APPLE1VIDEO_TERM_HEIGHT * APPLE1VIDEO_TERM_HEIGHT);
}

bool Apple1Video::setResource(const string &name, const OEData *data)
{
	if (name == "image")
		characterSet = *data;
	else
		return false;
	
	return true;
}

bool Apple1Video::connect(const string &name, OEComponent *component)
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

void Apple1Video::write(int address, int value)
{
}
