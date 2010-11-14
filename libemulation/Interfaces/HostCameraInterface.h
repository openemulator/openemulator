
/**
 * libemulation
 * Host camera interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host camera interface
 */

#ifndef _HOSTCAMERAINTERFACE_H
#define _HOSTCAMERAINTERFACE_H

typedef enum
{
	HOST_CAMERA_GET_FRAME,
} HostCameraMessages;

typedef struct
{
	char *frameData;
	int frameWidth;
	int frameHeight;
} HostCameraFrame;

#endif
