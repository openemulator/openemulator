
/**
 * libemulation
 * IEEE 488 Interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the IEEE 488 interface
 */

#ifndef _IEEE488INTERFACE
#define _IEEE488INTERFACE

typedef enum
{
	IEEE488_SEND_DATA,
} IEEE488Message;

typedef enum
{
	IEEE488_DID_RECEIVE_DATA,
} IEEE488Notification;

#endif
