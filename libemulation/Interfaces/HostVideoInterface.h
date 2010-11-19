
/**
 * libemulation
 * Host video interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host video interface
 */

#ifndef _HOSTVIDEOINTERFACE_H
#define _HOSTVIDEOINTERFACE_H

typedef enum
{
	HOST_VIDEO_GET_FRAME,
} HostVideoMessages;

typedef struct
{
	char *frameData;
	int frameWidth;
	int frameHeight;
} HostVideoFrame;

#endif
