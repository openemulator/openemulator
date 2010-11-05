
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

OEPAEmulation::OEPAEmulation(string path, string resourcesPath) :
OEEmulation(path, resourcesPath)
{
	oepa = NULL;
}

OEPAEmulation::OEPAEmulation(OEPA *oepa,
							 string path, string resourcesPath) :
OEEmulation(path, resourcesPath)
{
	this->oepa = oepa;
}

void OEPAEmulation::lock()
{
	if (oepa)
		((OEPA *)oepa)->lockEmulations();
}

void OEPAEmulation::unlock()
{
	if (oepa)
		((OEPA *)oepa)->unlockEmulations();
}

bool OEPAEmulation::save(string path)
{
	lock();
	
	bool status = OEEmulation::save(path);
	
	unlock();
	
	return status;
}

int OEPAEmulation::postMessage(string ref, int event, void *data)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	int status = 0;
	if (component)
		status = component->postMessage(component, event, data);
	else
		OEPALog("could not post event to '" + ref + "'");
	
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
