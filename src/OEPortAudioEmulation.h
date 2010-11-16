
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
	
	void setOEPortAudio(OEPortAudio *oePortAudio);
	
	bool open(string path);
	bool save(string path);
	
	bool addEDL(string path, OEIdMap connectionMap);
	bool removeDevice(string deviceName);
	
private:
	OEPortAudio *oePortAudio;
	
	void lock();
	void unlock();
};

#endif
