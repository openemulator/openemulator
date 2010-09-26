
/**
 * libemulation
 * Audio PLL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an audio phase locked loop
 */

#include "AudioPLL.h"

bool AudioPLL::setValue(string name, string value)
{
	if (name == "decisionFrequency")
		decisionFrequency = getFloat(value);
	else
		return false;
	
	return true;
}

bool AudioPLL::setComponent(string name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else
		return false;
	
	return true;
}
