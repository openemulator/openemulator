
/**
 * libemulation
 * Composite Monitor
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a composite monitor.
 */

#include "OEComponent.h"

#include "CanvasInterface.h"

class CompositeMonitor : public OEComponent
{
public:
	CompositeMonitor();
	
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
	bool init();
	
private:
	OEComponent *emulation;
	OEComponent *canvas;
	
	CanvasConfiguration configuration;
	OERect screenRect;
	
	string dummyPath;
	
	void updateContentRect();
};
