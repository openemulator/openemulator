
/**
 * libemulation
 * IEEE 844 Interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the IEEE 844 interface
 */

#ifndef _IEEE844INTERFACE
#define _IEEE844INTERFACE

typedef enum
{
	IEEE844_SEND_DATA,
} IEEE844Message;

typedef enum
{
	IEEE844_DID_RECEIVE_DATA,
} IEEE844Notification;

#endif
