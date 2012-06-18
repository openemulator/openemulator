
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

#include "OEEmulation.h"

#include "DeviceInterface.h"

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
    
    bool constructCanvas(OECanvasType canvasType, OEComponent **ref);
};

#endif
