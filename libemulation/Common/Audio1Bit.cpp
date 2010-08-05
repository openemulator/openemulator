
/**
 * libemulation
 * Audio 1-bit
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an audio 1-bit DAC and ADC
 */

#include "Audio1Bit.h"

bool Audio1Bit::connect(const string &name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else
		return false;
	
	return true;
}