
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

OEPortAudioEmulation::OEPortAudioEmulation() : OEEmulation()
{
	oePortAudio = NULL;
	
	setComponent("emulation", this);
}

void OEPortAudioEmulation::setOEPortAudio(OEPortAudio *oePortAudio)
{
	this->oePortAudio = oePortAudio;
	setComponent("hostAudio", oePortAudio);
}

void OEPortAudioEmulation::lock()
{
	if (oePortAudio)
		oePortAudio->lockEmulations();
}

void OEPortAudioEmulation::unlock()
{
	if (oePortAudio)
		oePortAudio->unlockEmulations();
}

bool OEPortAudioEmulation::open(string path)
{
	lock();
	
	bool status = OEEmulation::open(path);
	
	unlock();
	
	return status;
}

bool OEPortAudioEmulation::save(string path)
{
	lock();
	
	bool status = OEEmulation::save(path);
	
	unlock();
	
	return status;
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
