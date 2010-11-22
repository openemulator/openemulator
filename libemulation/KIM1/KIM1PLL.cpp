
/**
 * libemulation
 * KIM-1 PLL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the KIM-1 audio phase locked loop
 */

#include "KIM1PLL.h"
#include "HostAudioInterface.h"

KIM1PLL::KIM1PLL()
{
	hostAudio = NULL;
}

bool KIM1PLL::setValue(string name, string value)
{
	if (name == "decisionFrequency")
		decisionFrequency = getFloat(value);
	else
		return false;
	
	return true;
}

bool KIM1PLL::setRef(string name, OEComponent *ref)
{
	if (name == "hostAudio")
	{
		replaceObserver(hostAudio, ref, HOST_AUDIO_FRAME_WILL_BEGIN_RENDER);
		hostAudio = ref;
	}
	else
		return false;
	
	return true;
}
