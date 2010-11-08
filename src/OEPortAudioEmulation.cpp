
/**
 * OpenEmulator
 * OpenEmulator portaudio emulation interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator portaudio emulation interface.
 */

#include "OEPortAudioEmulation.h"
#include "OEPortAudio.h"

OEPortAudioEmulation::OEPortAudioEmulation(string path) :
OEEmulation()
{
	oeportaudio = NULL;
}

OEPortAudioEmulation::OEPortAudioEmulation(string path, string resourcesPath) :
OEEmulation(path, resourcesPath)
{
	oeportaudio = NULL;
}

OEPortAudioEmulation::OEPortAudioEmulation(OEPortAudio *oeportaudio,
										   string path, string resourcesPath) :
OEEmulation(path, resourcesPath)
{
	this->oeportaudio = oeportaudio;
}

void OEPortAudioEmulation::lock()
{
	if (oeportaudio)
		oeportaudio->lockEmulations();
}

void OEPortAudioEmulation::unlock()
{
	if (oeportaudio)
		oeportaudio->unlockEmulations();
}

bool OEPortAudioEmulation::save(string path)
{
	lock();
	
	bool status = OEEmulation::save(path);
	
	unlock();
	
	return status;
}

int OEPortAudioEmulation::postMessage(string ref, int message, void *data)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	int status = 0;
	if (component)
		status = component->postMessage(component, message, data);
	else
		OEPortAudioLog("could not post event to '" + ref + "'");
	
	unlock();
	
	return status;
}

void OEPortAudioEmulation::notify(string ref, int notification, void *data)
{
	lock();
	
	OEComponent *component = getComponent(ref);
	if (component)
		component->notify(NULL, notification, data);
	else
		OEPortAudioLog("could not send notification to '" + ref + "'");
	
	unlock();
}

bool OEPortAudioEmulation::addEDL(string path, OEIdMap deviceIdMap)
{
	lock();
	
	bool status = OEEmulation::addEDL(path, deviceIdMap);
	
	unlock();
	
	return status;
}

bool OEPortAudioEmulation::removeDevice(string id)
{
	lock();
	
	bool status = OEEDL::removeDevice(id);
	
	unlock();
	
	return status;
}
