
/**
 * libemulation
 * Terminal
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a terminal
 */

#include "Terminal.h"

bool Terminal::setValue(string name, string value)
{
	if (name == "width")
		width = getInt(value);
	else if (name == "height")
		height = getInt(value);
	else
		return false;
	
	return true;
}

bool Terminal::setComponent(string name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else
		return false;
	
	return true;
}
