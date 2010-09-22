
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

OEPAEmulation::OEPAEmulation(string path) :
OEEmulation()
{
	oepa = NULL;
}

OEPAEmulation::OEPAEmulation(string path, string resourcePath) :
OEEmulation(path, resourcePath)
{
	oepa = NULL;
}

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

bool OEPAEmulation::setValue(const string &ref, const string &name, string &value)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	bool status;
	if (component)
		status = component->setValue(name, value);
	else
	{
		status = false;
		oepaLog("could not set property " << name <<
				" (ref " << ref << " not found)");
	}
	
	unlock();
	
	return status;
}

bool OEPAEmulation::getValue(const string &ref, const string &name, string &value)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	bool status;
	if (component)
		status = component->getValue(name, value);
	else
	{
		status = false;
		oepaLog("could not get property " << name <<
				" (ref " << ref << " not found)");
	}
	
	unlock();
	
	return status;
}

void OEPAEmulation::notify(string ref, int notification, void *data)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	if (component)
		component->notify(NULL, notification, data);
	else
	{
		oepaLog("could not notify " + ref +
				" (ref not found)");
	}
	
	unlock();
}

int OEPAEmulation::postEvent(string ref, int message, void *data)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	int status = 0;
	if (component)
		status = component->postEvent(component, message, data);
	else
	{
		oepaLog("could not post event to " + ref +
				" (ref not found)");
	}
	
	unlock();
	
	return status;
}

bool OEPAEmulation::addEDL(string path, OEConnections &connections)
{
	lock();
	
	bool status = OEEmulation::addEDL(path, connections);
	
	unlock();
	
	return status;
}

bool OEPAEmulation::removeDevice(string deviceName)
{
	lock();
	
	bool status = OEEDL::removeDevice(deviceName);
	
	unlock();
	
	return status;
}
