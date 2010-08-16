
/**
 * libemulation
 * Generic serial port
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a generic serial port
 */

#include "OEComponent.h"

// Notifications
enum
{
	SERIAL_PORT_DATA_RECEIVED,
	SERIAL_PORT_BREAK_RECEIVED,
};

// Messages
enum
{
	SERIAL_PORT_BREAK,
};

class SerialPort : public OEComponent
{
};
