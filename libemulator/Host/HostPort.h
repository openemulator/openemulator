
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
	HOSTPORT_EVENT_SERIAL,
	HOSTPORT_EVENT_PARALLEL,
	HOSTPORT_EVENT_MIDI,
	HOSTPORT_EVENT_ETHERNET,
	HOSTPORT_EVENT_USB,
	HOSTPORT_EVENT_PDF,
};

class HostPort : public OEComponent
{
public:
	HostPort();
	
	bool addObserver(OEComponent *component, int notification);
	bool removeObserver(OEComponent *component, int notification);
	void postNotification(int notification, void *data);
	
	int ioctl(int message, void *data);
	
private:
	OEHostObserver hostObserver;
};
