
/**
 * libemulation
 * RS-232 Interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the RS-232 interface
 */

// Notes:
// * Data is sent and received as OEData.

#ifndef _RS232INTERFACE
#define _RS232INTERFACE

typedef enum
{
	RS232_SEND_DATA,
	
    RS232_ASSERT_RTS,
	RS232_CLEAR_RTS,
    RS232_IS_CTS_ASSERTED,
    
	RS232_ASSERT_DTR,
	RS232_CLEAR_DTR,
    RS232_IS_DSR_ASSERTED,
    
    RS232_IS_DCD_ASSERTED,
    RS232_IS_RI_ASSERTED,
    
	RS232_SEND_BREAK,
} RS232Message;

typedef enum
{
	RS232_DID_RECEIVE_DATA,
	
    RS232_CTS_DID_CHANGE,
    RS232_DSR_DID_CHANGE,
    RS232_DCD_DID_CHANGE,
	RS232_RI_DID_CHANGE,
} RS232Notification;

#endif
