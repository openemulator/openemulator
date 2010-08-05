
/**
 * libemulation
 * Audio PLL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an audio phase locked loop
 */

#include "AudioPLL.h"

bool AudioPLL::setProperty(const string &name, const string &value)
{
	if (name == "frequency")
		frequency = getInt(value);
	else
		return false;
	
	return true;
}

bool AudioPLL::connect(const string &name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else
		return false;
	
	return true;
}
