
/**
 * libemulation
 * Audio 1-bit
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an audio 1-bit DAC and ADC
 */

#include "OEComponent.h"

class Audio1Bit : public OEComponent
{
public:
	bool connect(const string &name, OEComponent *component);

private:
	OEComponent *host;
};
