
/**
 * libemulation
 * KIM-1 I/O
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements the KIM-1 input/output
 */

#include "KIM1IO.h"

#include "SerialPort.h"

KIM1IO::KIM1IO()
{
	host = NULL;
	serialPort = NULL;
	audioOut = NULL;
	audioIn = NULL;
	
	view = NULL;
}

KIM1IO::~KIM1IO()
{
	delete view;
}

bool KIM1IO::setComponent(string name, OEComponent *component)
{
	if (name == "host")
	{
		if (host)
		{
			host->postEvent(this, HOST_REMOVE_SCREEN, screen);
			host->removeObserver(this, HOST_HID_SYSTEM_CHANGED);
		}
		host = component;
		if (host)
		{
			host->postEvent(this, HOST_ADD_SCREEN, screen);
			host->addObserver(this, HOST_HID_SYSTEM_CHANGED);
		}
	}
	else if (name == "serialPort")
	{
		if (serialPort)
			serialPort->removeObserver(this, SERIAL_PORT_DATA_RECEIVED);
		serialPort = component;
		if (serialPort)
			serialPort->addObserver(this, SERIAL_PORT_DATA_RECEIVED);
	}
	else if (name == "audioOut")
		audioOut = component;
	else if (name == "audioIn")
		audioIn = component;
	else
		return false;
	
	return true;
}

bool KIM1IO::setData(string name, OEData *data)
{
	if (name == "view")
		view = data;
	else
		return false;
	
	return true;
}

void KIM1IO::notify(OEComponent *component, int notification, void *data)
{

}