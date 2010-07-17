/*
 *  OEPAEmulation.cpp
 *  OpenEmulator
 *
 *  Created by Marc S. Ressl on 7/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "OEPA.h"

OEPAEmulation::OEPAEmulation(OEPA *oepa, string path, string resourcePath) :
OEEmulation(path, resourcePath), oepa(oepa)
{
}

bool OEPAEmulation::save(string path)
{
	oepa->lockProcess();
	
	bool status = OEEmulation::save(path);
	
	oepa->unlockProcess();
	
	return status;
}

bool OEPAEmulation::setProperty(string ref, string name, string value)
{
	oepa->lockProcess();
	
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
	
	oepa->unlockProcess();
	
	return status;
}

bool OEPAEmulation::getProperty(string ref, string name, string &value)
{
	oepa->lockProcess();
	
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
	
	oepa->unlockProcess();
	
	return status;
}

void OEPAEmulation::postNotification(string ref, int notification, void *data)
{
	oepa->lockProcess();
	
	OEComponent *component = getComponent(ref);
	if (component)
		component->postNotification(notification, data);
	else
	{
		oepaLog("could not post notification to " << ref <<
				" (ref not found)");
	}
	
	oepa->unlockProcess();
}

int OEPAEmulation::ioctl(string ref, int message, void *data)
{
	oepa->lockProcess();
	
	OEComponent *component = getComponent(ref);
	int status = 0;
	if (component)
		status = component->ioctl(message, data);
	else
	{
		oepaLog("could not ioctl " << ref <<
				" (ref not found)");
	}
	
	oepa->unlockProcess();
	
	return status;
}

bool OEPAEmulation::addDevices(string path, OEStringRefMap connections)
{
	oepa->lockProcess();
	
	bool status = OEEmulation::addDevices(path, connections);
	
	oepa->unlockProcess();
	
	return status;
}

bool OEPAEmulation::isDeviceTerminal(OERef ref)
{
	oepa->lockProcess();
	
	bool status = OEEmulation::isDeviceTerminal(ref);
	
	oepa->unlockProcess();
	
	return status;
}

bool OEPAEmulation::removeDevice(OERef ref)
{
	oepa->lockProcess();
	
	bool status = OEEmulation::removeDevice(ref);
	
	oepa->unlockProcess();
	
	return status;
}
