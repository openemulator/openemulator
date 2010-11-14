
/**
 * libemulation
 * Host device status interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host device status interface
 */

#ifndef _HOSTDEVICESTATUSINTERFACE_H
#define _HOSTDEVICESTATUSINTERFACE_H

typedef enum
{
	HOST_DEVICESTATUS_SET_INFOLINE,
	HOST_DEVICESTATUS_SET_CANVAS_COMPONENT,
	HOST_DEVICESTATUS_SET_STORAGE_COMPONENT,
} HostDeviceStatusMessages;

#endif
