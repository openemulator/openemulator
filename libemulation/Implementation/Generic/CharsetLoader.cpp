
/**
 * libemulation
 * Character set loader
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Loads and parses a character set.
 */

#include "CharsetLoader.h"

bool CharsetLoader::setValue(string name, string value)
{
	return true;
}

bool CharsetLoader::setData(string name, OEData *data)
{
	if (name == "image")
		;
	else
		return false;
	
	return true;
}
