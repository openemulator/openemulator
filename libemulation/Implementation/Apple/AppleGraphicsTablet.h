
/**
 * libemulation
 * Apple Graphics Tablet
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Graphics Tablet.
 */

#include "OEComponent.h"

class AppleGraphicsTablet : public OEComponent
{
public:
	AppleGraphicsTablet();
	
	bool setRef(string name, OEComponent *ref);
	
private:
	OEComponent *emulation;
	
	OEComponent *canvas;
};
