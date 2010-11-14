
/**
 * libemulation
 * IEEE 1284 Interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the IEEE 1284 interface
 */

typedef enum
{
	IEEE1284_SET_STROBE,
	IEEE1284_SET_LF,
	IEEE1284_SET_INIT,
	IEEE1284_SET_SELECTIN,
} IEEE1284Messages;

typedef enum
{
	IEEE1284_DATA_SENT,
	IEEE1284_ACK_CHANGED,
	IEEE1284_BUSY_CHANGED,
	IEEE1284_PE_CHANGED,
	IEEE1284_SELECT_CHANGED,
	IEEE1284_FAULT_CHANGED,
} IEEE1284Notifications;
