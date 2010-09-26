
/**
 * libemulation
 * Apple I Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Video
 */

#include "Apple1Terminal.h"

Apple1Terminal::Apple1Terminal()
{
	memset(screen, 0, APPLE1TERMINAL_DEFAULT_HEIGHT * APPLE1TERMINAL_DEFAULT_WIDTH);
}

bool Apple1Terminal::setComponent(string name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else if (name == "charset")
		charset = component;
	else if (name == "monitor")
		monitor = component;
	else
		return false;
	
	return true;
}
