
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

	HOSTPORT_OPEN,
	HOSTPORT_CLOSE,
	HOSTPORT_WRITE,
	
	HOSTPORT_ADD_SCREEN,
	HOSTPORT_REMOVE_SCREEN,
	HOSTPORT_UPDATE_SCREEN,
};

typedef void (*HostPortWrite)(char *data, int size);
typedef bool (*HostPortOpen)(int device);
typedef void (*HostPortClose)(int device);

typedef struct
{
	HostPortOpen open;
	HostPortClose close;
	HostPortWrite serial1;
	HostPortWrite serial2;
	HostPortWrite serial3;
	HostPortWrite serial4;
	HostPortWrite parallel;
	HostPortWrite midi;
	HostPortWrite ethernet1;
	HostPortWrite ethernet2;
	HostPortWrite usb;
	HostPortWrite pdf;
} HostPortObserver;

// Notifications
enum
{
	HOSTPORT_WRITE_SERIAL1,
	HOSTPORT_WRITE_SERIAL2,
	HOSTPORT_WRITE_SERIAL3,
	HOSTPORT_WRITE_SERIAL4,
	HOSTPORT_WRITE_PARALLEL,
	HOSTPORT_WRITE_MIDI,
	HOSTPORT_WRITE_ETHERNET1,
	HOSTPORT_WRITE_ETHERNET2,
	HOSTPORT_WRITE_USB,
	HOSTPORT_WRITE_PDF,
};

class HostPort : public OEComponent
{
public:
	HostPort();
	
	int ioctl(int message, void *data);
	
private:
	HostPortObserver observer;
	OEComponent *serial1;
	OEComponent *serial2;
	OEComponent *serial3;
	OEComponent *serial4;
	OEComponent *parallel;
	OEComponent *ethernet1;
	OEComponent *ethernet2;
	OEComponent *usb;
};
