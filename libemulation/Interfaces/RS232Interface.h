
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
	RS232_SET_DTR,
	RS232_SET_RTS,
	RS232_SEND_BREAK,
} RS232Messages;

typedef enum
{
	RS232_DATA_RECEIVED,
	RS232_BREAK_ASSERTED,
	RS232_DSR_CHANGED,
	RS232_CTS_CHANGED,
	RS232_DCD_CHANGED,
	RS232_RI_CHANGED,
} RS232Notifications;
