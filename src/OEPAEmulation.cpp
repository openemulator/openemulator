
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

bool OEPAEmulation::setValue(string ref, string name, string value)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	bool status;
	if (component)
		status = component->setValue(name, value);
	else
	{
		status = false;
		OEPALog("could not set property '" + name + "' for '" + ref + "'");
	}
	
	unlock();
	
	return status;
}

bool OEPAEmulation::getValue(string ref, string name, string &value)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	bool status;
	if (component)
		status = component->getValue(name, value);
	else
	{
		status = false;
		OEPALog("could not get property '" + name + "' for '" + ref + "'");
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
		OEPALog("could not send notification to '" + ref + "'");
	
	unlock();
}

int OEPAEmulation::postEvent(string ref, int event, void *data)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	int status = 0;
	if (component)
		status = component->postEvent(component, event, data);
	else
		OEPALog("could not post event to '" + ref + "'");
	
	unlock();
	
	return status;
}

bool OEPAEmulation::addEDL(string path, OEIdMap deviceIdMap)
{
	lock();
	
	bool status = OEEmulation::addEDL(path, deviceIdMap);
	
	unlock();
	
	return status;
}

bool OEPAEmulation::removeDevice(string id)
{
	lock();
	
	bool status = OEEDL::removeDevice(id);
	
	unlock();
	
	return status;
}
