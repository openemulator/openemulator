
/**
 * OpenEmulator
 * OpenEmulator portaudio emulation interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator portaudio emulation interface.
 */

#ifndef _OEPORTAUDIOEMULATION_H
#define _OEPORTAUDIOEMULATION_H

#include "OEEmulation.h"

class OEPortAudio;

class OEPortAudioEmulation : public OEEmulation
{
public:
	OEPortAudioEmulation();
	OEPortAudioEmulation(string path);
	OEPortAudioEmulation(string path, string resourcesPath);
	OEPortAudioEmulation(OEPortAudio *oeportaudio,
						 string path, string resourcesPath);
	
	bool open(string path);
	bool save(string path);
	
	int postMessage(string ref, int event, void *data);
	void notify(string ref, int notification, void *data);
	
	bool addEDL(string path, OEIdMap deviceIdMap);
	bool removeDevice(string deviceName);
	
private:
	OEPortAudio *oeportaudio;
	
	void lock();
	void unlock();
};

typedef vector<OEPortAudioEmulation *> OEPortAudioEmulations;

#endif
