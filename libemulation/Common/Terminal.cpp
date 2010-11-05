
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

bool Terminal::setRef(string name, OEComponent *ref)
{
	if (name == "charset")
		charset = ref;
	else
		return false;
	
	return true;
}
