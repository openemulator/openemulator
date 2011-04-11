
/**
 * libemulation
 * Apple Graphics Tablet
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Graphics Tablet.
 */

#include "OEComponent.h"
#include "OEImage.h"

class AppleGraphicsTablet : public OEComponent
{
public:
	AppleGraphicsTablet();
	
	bool setValue(string name, string value);
	bool setRef(string name, OEComponent *ref);
	bool init();
	
private:
	OEComponent *device;
	OEComponent *canvas;
	
	string viewPath;
};
