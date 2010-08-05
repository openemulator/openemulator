
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
	if (name == "floatingBus")
		floatingBus = getInt(value);
	else if (name == "crystal")
	{
		crystal = getInt(value);
		updateFrequency();
	}
	else if (name == "divider")
	{
		divider = getInt(value);
		updateFrequency();
	}
	else if (name == "resetOnPowerOn")
		resetOnPowerOn = getInt(value);
	else
		return false;
	
	return true;
}
