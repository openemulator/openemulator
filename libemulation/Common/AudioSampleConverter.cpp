
/**
 * libemulation
 * Audio sample converter
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an audio sample converter (8 and 16 bit)
 */

#include "AudioSampleConverter.h"

bool AudioSampleConverter::setRef(string name, OEComponent *ref)
{
	if (name == "controlBus")
		controlBus = ref;
	else
		return false;
	
	return true;
}
