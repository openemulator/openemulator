
/**
 * libemulation
 * KIM-1 PLL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the KIM-1 audio phase locked loop
 */

#include "OEComponent.h"

class KIM1PLL : public OEComponent
{
public:
	bool setValue(string name, string value);
	bool setComponent(string name, OEComponent *component);

private:
	OEComponent *host;
	
	float decisionFrequency;
};
