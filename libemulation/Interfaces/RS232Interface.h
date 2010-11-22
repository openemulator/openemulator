
/**
 * libemulation
 * RS-232 Interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the RS-232 interface
 */

typedef enum
{
	RS232_SEND_DATA,
	RS232_SET_DTR,
	RS232_SET_RTS,
	RS232_SEND_BREAK,
} RS232Messages;

typedef enum
{
	RS232_DID_RECEIVE_DATA,
	RS232_DSR_DID_CHANGE,
	RS232_CTS_DID_CHANGE,
	RS232_DCD_DID_CHANGE,
	RS232_RI_DID_CHANGE,
	RS232_BREAK_DID_ASSERT,
} RS232Notifications;
