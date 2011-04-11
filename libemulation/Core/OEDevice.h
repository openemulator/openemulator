
/**
 * libemulation
 * OEDevice
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device.
 */

#ifndef _OEDEVICE_H
#define _OEDEVICE_H

#include "OEComponent.h"

#include "DeviceInterface.h"

class OEDevice : public OEComponent
{
public:
	OEDevice();
	
	bool setRef(string name, OEComponent *ref);
	bool init();
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	OEComponent *emulation;
	
	string label;
	string imagePath;
	string group;
	string locationLabel;
	string stateLabel;
	
	DeviceSettings settings;
	
	OEComponents canvases;
	OEComponent *storage;
};

#endif
