
/**
 * libemulation
 * MOSKIM-1 PLL
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the MOSKIM-1 audio phase locked loop
 */

#include "OEComponent.h"

class MOSKIM1PLL : public OEComponent
{
public:
	MOSKIM1PLL();
	
	bool setValue(string name, string value);
	bool setRef(string name, OEComponent *ref);
	
private:
	OEComponent *audio;
	
	float decisionFrequency;
};
