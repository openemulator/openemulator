
/**
 * OpenEmulator
 * OEPA Emulation interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OEPA Emulation interface.
 */

#include "OEPAEmulation.h"
#include "OEPA.h"

OEPAEmulation::OEPAEmulation(OEPA *oepa,
							 string path, string resourcePath) :
OEEmulation(path, resourcePath)
{
	this->oepa = oepa;
}

void OEPAEmulation::lock()
{
	((OEPA *)oepa)->lockEmulations();
}

void OEPAEmulation::unlock()
{
	((OEPA *)oepa)->unlockEmulations();
}

bool OEPAEmulation::save(string path)
{
	lock();
	
	bool status = OEEmulation::save(path);
	
	unlock();
	
	return status;
}

bool OEPAEmulation::setProperty(string ref, string name, string value)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	bool status;
	if (component)
		status = component->setProperty(name, value);
	else
	{
		status = false;
		oepaLog("could not set property " << ref << "." << name <<
				" (ref not found)");
	}
	
	unlock();
	
	return status;
}

bool OEPAEmulation::getProperty(string ref, string name, string &value)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	bool status;
	if (component)
		status = component->getProperty(name, value);
	else
	{
		status = false;
		oepaLog("could not get property " << ref << "." << name << 
				" (ref not found)");
	}
	
	unlock();
	
	return status;
}

void OEPAEmulation::postNotification(string ref, int notification, void *data)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	if (component)
		component->postNotification(notification, data);
	else
	{
		oepaLog("could not post notification to " << ref <<
				" (ref not found)");
	}
	
	unlock();
}

int OEPAEmulation::ioctl(string ref, int message, void *data)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	int status = 0;
	if (component)
		status = component->ioctl(message, data);
	else
	{
		oepaLog("could not ioctl " << ref <<
				" (ref not found)");
	}
	
	unlock();
	
	return status;
}

bool OEPAEmulation::addDevices(string path, OEStringRefMap connections)
{
	lock();
	
	bool status = OEEmulation::addDevices(path, connections);
	
	unlock();
	
	return status;
}

bool OEPAEmulation::isDeviceTerminal(OERef ref)
{
	lock();
	
	bool status = OEEmulation::isDeviceTerminal(ref);
	
	unlock();
	
	return status;
}

bool OEPAEmulation::removeDevice(OERef ref)
{
	lock();
	
	bool status = OEEmulation::removeDevice(ref);
	
	unlock();
	
	return status;
}
