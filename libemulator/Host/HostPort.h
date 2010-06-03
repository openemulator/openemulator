
/**
 * libemulator
 * Host Port
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host ports (serial, parallel, midi, ethernet, usb, pdf)
 */

#include "OEComponent.h"

// Messages
enum
{
	HOSTPORT_REGISTER_HOST,
};

// Notifications
enum
{
	HOSTPORT_READ_SERIAL1,
	HOSTPORT_WRITE_SERIAL1,
	HOSTPORT_READ_SERIAL2,
	HOSTPORT_WRITE_SERIAL2,
	HOSTPORT_READ_SERIAL3,
	HOSTPORT_WRITE_SERIAL3,
	HOSTPORT_READ_SERIAL4,
	HOSTPORT_WRITE_SERIAL4,
	HOSTPORT_READ_PARALLEL,
	HOSTPORT_WRITE_PARALLEL,
	HOSTPORT_WRITE_MIDI,
	HOSTPORT_READ_ETHERNET1,
	HOSTPORT_WRITE_ETHERNET1,
	HOSTPORT_READ_ETHERNET2,
	HOSTPORT_WRITE_ETHERNET2,
	HOSTPORT_READ_USB,
	HOSTPORT_WRITE_USB,
	HOSTPORT_WRITE_PDF,
};

class HostPort : public OEComponent
{
public:
	HostPort();
	
	bool addObserver(OEComponent *component, int notification);
	bool removeObserver(OEComponent *component, int notification);
	
	int ioctl(int message, void *data);
	
private:
	OEHostObserver hostObserver;
};
