
/**
 * libemulation
 * Composite Monitor
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
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
	
	bool setRef(string name, OEComponent *ref);
	
private:
	OEComponent *emulation;
	
	OEComponent *canvas;
	
};
