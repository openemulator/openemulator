
/**
 * libemulation
 * Audio PLL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an audio phase locked loop
 */

#include "OEComponent.h"

class AudioPLL : public OEComponent
{
public:
	bool setProperty(const string &name, const string &value);
	bool connect(const string &name, OEComponent *component);

private:
	OEComponent *host;
	
	float frequency;
};
