
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
	RS232_RECEIVE_DATA,
	
    RS232_IS_RTS,
    RS232_SET_CTS,
    
    RS232_IS_DTR,
    RS232_SET_DSR,

    RS232_SET_DCD,
    RS232_SET_RI,
    
	RS232_SEND_RECEIVEBREAK,
} RS232DTEMessage;

typedef enum
{
	RS232_TRANSMIT_DATA,
	
    RS232_SET_RTS,
    RS232_IS_CTS,
    
	RS232_SET_DTR,
    RS232_IS_DSR,
    
    RS232_IS_DCD,
    RS232_IS_RI,
    
	RS232_SEND_TRANSMITBREAK,
} RS232DCEMessage;

#endif
