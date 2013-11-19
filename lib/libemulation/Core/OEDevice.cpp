
/**
 * libemulation
 * OEDevice
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device
 */

#include "OEDevice.h"

#include "OEEmulation.h"

#include "EmulationInterface.h"

OEDevice::OEDevice(OEEmulation *emulation)
{
    this->emulation = emulation;
}

bool OEDevice::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case DEVICE_SET_LABEL:
            if (data)
                label = *((string *)data);
            
            return true;
            
        case DEVICE_GET_LABEL:
            if (data)
                *((string *)data) = label;
            
            return true;
            
        case DEVICE_SET_IMAGEPATH:
            if (data)
                imagePath = *((string *)data);
            
            return true;
            
        case DEVICE_GET_IMAGEPATH:
            if (data)
                *((string *)data) = imagePath;
            
            return true;
            
        case DEVICE_SET_LOCATIONLABEL:
            if (data)
                locationLabel = *((string *)data);
            
            return true;
            
        case DEVICE_GET_LOCATIONLABEL:
            if (data)
                *((string *)data) = locationLabel;
            
            return true;
            
        case DEVICE_SET_STATELABEL:
            if (data)
                stateLabel = *((string *)data);
            
            return true;
            
        case DEVICE_GET_STATELABEL:
            if (data)
                *((string *)data) = stateLabel;
            
            return true;
            
        case DEVICE_SET_SETTINGS:
            if (data)
                settings = *((DeviceSettings *)data);
            
            return true;
            
        case DEVICE_GET_SETTINGS:
            if (data)
                *((DeviceSettings *)data) = settings;
            
            return true;
            
        case DEVICE_CONSTRUCT_DISPLAYCANVAS:
            if (!emulation->postMessage(this, EMULATION_CONSTRUCT_DISPLAYCANVAS, data))
                return false;
            
            canvases.push_back(*((OEComponent **)data));
            
            return true;
            
        case DEVICE_CONSTRUCT_PAPERCANVAS:
            if (!emulation->postMessage(this, EMULATION_CONSTRUCT_PAPERCANVAS, data))
                return false;
            
            canvases.push_back(*((OEComponent **)data));
            
            return true;
            
        case DEVICE_CONSTRUCT_OPENGLCANVAS:
            if (!emulation->postMessage(this, EMULATION_CONSTRUCT_PAPERCANVAS, data))
                return false;
            
            canvases.push_back(*((OEComponent **)data));
            
            return true;
            
        case DEVICE_DESTROY_CANVAS:
            
            if (!emulation->postMessage(this, EMULATION_DESTROY_CANVAS, data))
                return false;
            else
            {
                OEComponents::iterator first = canvases.begin();
                OEComponents::iterator last = canvases.end();
                OEComponents::iterator i = remove(first, last, *((OEComponent **)data));
                
                if (i != last)
                    canvases.erase(i, last);
                
                return true;
            }
            
        case DEVICE_GET_CANVASES:
            *((OEComponents *)data) = canvases;
            
            return true;
            
        case DEVICE_ADD_STORAGE:
            storages.push_back((OEComponent *)data);
            
            return true;
            
        case DEVICE_REMOVE_STORAGE:
        {
            OEComponents::iterator first = storages.begin();
            OEComponents::iterator last = storages.end();
            OEComponents::iterator i = remove(first, last, (OEComponent *)data);
            
            if (i != last)
                storages.erase(i, last);
            
            return true;
        }            
        case DEVICE_GET_STORAGES:
            *((OEComponents *)data) = storages;
            
            return true;
            
        case DEVICE_UPDATE:
            return emulation->postMessage(sender, EMULATION_UPDATE, data);
    }
    
    return false;
}
