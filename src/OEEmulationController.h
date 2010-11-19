
/**
 * OpenEmulator
 * OpenEmulator emulation controller
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator emulation controller object.
 */

#ifndef _OEEMULATIONCONTROLLER_H
#define _OEEMULATIONCONTROLLER_H

#include "OEComponent.h"

typedef void (*OERunAlertCallback)();

class OEEmulationController: public OEComponent
{
public:
	OEEmulationController();
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
	void setAlertCallback(OERunAlertCallback alertCallback);
	bool mount(string path);
	bool mount(string deviceId, string path);
	bool validate(string path);
	
private:
	OERunAlertCallback alertCallback;
};

#endif
