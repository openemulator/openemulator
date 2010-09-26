
/**
 * libemulation
 * Audio sample converter
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an audio sample converter (8 and 16 bit)
 */

#include "OEComponent.h"

class AudioSampleConverter : public OEComponent
{
public:
	bool setComponent(string name, OEComponent *component);

private:
	OEComponent *host;
	OEComponent *controlBus;
};
