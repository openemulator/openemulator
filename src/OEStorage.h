
/**
 * OpenEmulator
 * OpenEmulator storage
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator storage object.
 */

#ifndef _OESTORAGE_H
#define _OESTORAGE_H

#include "OEComponent.h"
#include "HostStorageInterface.h"

typedef void (*OEStorageAlertCallback)();

class OEStorage : public OEComponent
{
public:
	OEStorage();
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
	void setAlertCallback(OEStorageAlertCallback alertCallback);
	bool mount(string path);
	bool validate(string path);
	
private:
	OEStorageAlertCallback alertCallback;
};

#endif
