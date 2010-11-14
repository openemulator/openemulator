
/**
 * libemulation
 * Host ethernet interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host ethernet interface
 */

#ifndef _HOSTETHERNETINTERFACE_H
#define _HOSTETHERNETINTERFACE_H

typedef enum
{
	HOST_ETHERNET_SEND,
	HOST_ETHERNET_RECEIVE,
} HostEthernetMessages;

typedef struct
{
	char *buffer;
	int bufferSize;
	int bufferUsed;
} HostEthernetBuffer;

#endif
