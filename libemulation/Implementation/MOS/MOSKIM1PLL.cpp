
/**
 * libemulation
 * MOSKIM-1 PLL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the MOSKIM-1 audio phase locked loop
 */

#include "MOSKIM1PLL.h"
#include "AudioInterface.h"

MOSKIM1PLL::MOSKIM1PLL()
{
	audio = NULL;
}

bool MOSKIM1PLL::setValue(string name, string value)
{
	if (name == "decisionFrequency")
		decisionFrequency = getFloat(value);
	else
		return false;
	
	return true;
}

bool MOSKIM1PLL::setRef(string name, OEComponent *ref)
{
	if (name == "audio")
	{
		setObserver(audio, ref, AUDIO_FRAME_WILL_RENDER);
		audio = ref;
	}
	else
		return false;
	
	return true;
}
