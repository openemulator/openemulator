
/**
 * libemulation
 * Host device status interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host device status interface
 */

#ifndef _HOSTDEVICESTATEINTERFACE_H
#define _HOSTDEVICESTATEINTERFACE_H

typedef enum
{
	HOST_DEVICESTATE_SET,
} HostDeviceStateMessages;

typedef struct
{
	string informativeText;
	OEComponent *canvas;
	OEComponent *storage;
	bool isStorageMounted;
} HostDeviceState;

typedef struct
{
	string deviceId;
	HostDeviceState state;
} HostDeviceStateData;

#endif
