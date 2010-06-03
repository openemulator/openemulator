
/**
 * libemulator
 * Host Port
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host ports (serial, parallel, midi, ethernet, usb, pdf)
 */

#include "HostPort.h"

HostPort::HostPort()
{
	hostObserver.addObserver = NULL;
	hostObserver.removeObserver = NULL;
	hostObserver.notify = NULL;
}

bool HostPort::addObserver(OEComponent *component, int notification)
{
	hostObserver.addObserver(notification);
	return OEComponent::addObserver(component, notification);
}

bool HostPort::removeObserver(OEComponent *component, int notification)
{
	hostObserver.removeObserver(notification);
	return OEComponent::removeObserver(component, notification);
}

int HostPort::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTPORT_REGISTER_HOST:
		{
			if (hostObserver.notify)
			{
				removeObserver(this, HOSTPORT_WRITE_SERIAL1);
				removeObserver(this, HOSTPORT_WRITE_SERIAL2);
				removeObserver(this, HOSTPORT_WRITE_SERIAL3);
				removeObserver(this, HOSTPORT_WRITE_SERIAL4);
				removeObserver(this, HOSTPORT_WRITE_PARALLEL);
				removeObserver(this, HOSTPORT_WRITE_MIDI);
				removeObserver(this, HOSTPORT_WRITE_ETHERNET1);
				removeObserver(this, HOSTPORT_WRITE_ETHERNET2);
				removeObserver(this, HOSTPORT_WRITE_USB);
				removeObserver(this, HOSTPORT_WRITE_PDF);
			}
			hostObserver = *((OEHostObserver *) data);
			if (hostObserver.notify)
			{
				addObserver(this, HOSTPORT_WRITE_SERIAL1);
				addObserver(this, HOSTPORT_WRITE_SERIAL2);
				addObserver(this, HOSTPORT_WRITE_SERIAL3);
				addObserver(this, HOSTPORT_WRITE_SERIAL4);
				addObserver(this, HOSTPORT_WRITE_PARALLEL);
				addObserver(this, HOSTPORT_WRITE_MIDI);
				addObserver(this, HOSTPORT_WRITE_ETHERNET1);
				addObserver(this, HOSTPORT_WRITE_ETHERNET2);
				addObserver(this, HOSTPORT_WRITE_USB);
				addObserver(this, HOSTPORT_WRITE_PDF);
			}
			return true;
		}
	}
	
	return false;
}
