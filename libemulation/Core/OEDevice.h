
/**
 * libemulation
 * OEDevice
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device
 */

#ifndef _OEDEVICE_H
#define _OEDEVICE_H

#include "OEComponent.h"

#include "DeviceInterface.h"

class OEEmulation;

class OEDevice : public OEComponent
{
public:
    OEDevice(OEEmulation *emulation);
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
    OEEmulation *emulation;
    
    string label;
    string imagePath;
    string locationLabel;
    string stateLabel;
    
    DeviceSettings settings;
    
    OEComponents canvases;
    OEComponents storages;
};

#endif
