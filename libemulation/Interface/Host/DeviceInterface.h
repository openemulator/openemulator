
/**
 * libemulation
 * Device interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the device interface
 */

#ifndef _DEVICEINTERFACE_H
#define _DEVICEINTERFACE_H

#include <string>
#include <vector>

#include "OEComponent.h"

using namespace std;

typedef enum
{
    DEVICE_SET_LABEL,
    DEVICE_GET_LABEL,
    DEVICE_SET_IMAGEPATH,
    DEVICE_GET_IMAGEPATH,
    DEVICE_SET_LOCATIONLABEL,
    DEVICE_GET_LOCATIONLABEL,
    DEVICE_SET_STATELABEL,
    DEVICE_GET_STATELABEL,
    
    DEVICE_SET_SETTINGS,
    DEVICE_GET_SETTINGS,
    
    DEVICE_CONSTRUCT_DISPLAYCANVAS,
    DEVICE_CONSTRUCT_PAPERCANVAS,
    DEVICE_CONSTRUCT_OPENGLCANVAS,
    DEVICE_DESTROY_CANVAS,
    DEVICE_GET_CANVASES,
    
    DEVICE_ADD_STORAGE,
    DEVICE_REMOVE_STORAGE,
    DEVICE_GET_STORAGES,
    
    DEVICE_UPDATE,
} DeviceMessage;

typedef struct
{
    OEComponent *component;
    string name;
    string label;
    string type;
    string options;
} DeviceSetting;

typedef vector<DeviceSetting> DeviceSettings;

#endif
