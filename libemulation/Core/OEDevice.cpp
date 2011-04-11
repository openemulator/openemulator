
/**
 * libemulation
 * OEDevice
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a device.
 */

#include "OEDevice.h"

#include "EmulationInterface.h"

OEDevice::OEDevice()
{
	storage = NULL;
}

bool OEDevice::setRef(string name, OEComponent *ref)
{
	if (name == "emulation")
		emulation = (OEComponent *)ref;
	else
		return false;
	
	return true;
}

bool OEDevice::init()
{
	if (!emulation)
	{
		printLog("ref 'emulation' undefined");
		return false;
	}
	
	return true;
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
		case DEVICE_SET_GROUP:
			if (data)
				group = *((string *)data);
			return true;
		case DEVICE_GET_GROUP:
			if (data)
				*((string *)data) = group;
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
				*((DeviceSettings **)data) = &settings;
			return true;
			
		case DEVICE_HAS_CANVASES:
			if (data)
				*((bool *)data) = canvases.size();
			return true;
		case DEVICE_CREATE_CANVAS:
			if (data)
			{
				OEComponent **ref = (OEComponent **)data;
				
				if (emulation->postMessage(this, EMULATION_CREATE_CANVAS, ref))
				{
					canvases.push_back(*ref);
					
					return true;
				}
			}
			return false;
		case DEVICE_DESTROY_CANVAS:
			if (data)
			{
				OEComponent **ref = (OEComponent **)data;
				
				OEComponents::iterator first = canvases.begin();
				OEComponents::iterator last = canvases.end();
				OEComponents::iterator i = remove(first, last, *ref);
				if (i != last)
					canvases.erase(i, last);
				
				return emulation->postMessage(this, EMULATION_DESTROY_CANVAS, ref);
			}
			break;
			
		case DEVICE_POST_SYSTEMEVENT:
			notify(sender, DEVICE_SYSTEMEVENT_DID_OCCUR, data);
			return true;
	}
	
	return false;
}
