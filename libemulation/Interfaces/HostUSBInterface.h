
/**
 * libemulation
 * Host USB interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host USB interface
 */

#ifndef _HOSTUSBINTERFACE_H
#define _HOSTUSBINTERFACE_H

typedef enum
{
	HOST_USB_SEND,
	HOST_USB_RECEIVE,
} HostUSBMessages;

typedef struct
{
	char *buffer;
	int bufferSize;
	int bufferUsed;
} HostUSBBuffer;

#endif
