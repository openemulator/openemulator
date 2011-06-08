
/**
 * libemulation
 * Apple Silentype
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Silentype printer
 */

#include "OEComponent.h"
#include "OEImage.h"

class AppleSilentype : public OEComponent
{
public:
	AppleSilentype();
	
	bool setValue(string name, string value);
	bool setRef(string name, OEComponent *ref);
	bool init();
	
private:
	OEComponent *device;
	OEComponent *canvas;
	
	string viewPath;
};
