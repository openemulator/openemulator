
/**
 * libemulation
 * KIM-1 PLL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the KIM-1 audio phase locked loop
 */

#include "KIM1PLL.h"

bool KIM1PLL::setValue(string name, string value)
{
	if (name == "decisionFrequency")
		decisionFrequency = getFloat(value);
	else
		return false;
	
	return true;
}

bool KIM1PLL::setComponent(string name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else
		return false;
	
	return true;
}
