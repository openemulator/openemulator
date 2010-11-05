
/**
 * OpenEmulator
 * OEPA Emulation interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OEPA Emulation interface.
 */

#ifndef _OEPAEMULATION_H
#define _OEPAEMULATION_H

#include "OEEmulation.h"

class OEPA;

class OEPAEmulation : public OEEmulation
{
public:
	OEPAEmulation();
	OEPAEmulation(string path);
	OEPAEmulation(string path, string resourcesPath);
	OEPAEmulation(OEPA *oepa, string path, string resourcesPath);
	
	bool open(string path);
	bool save(string path);
	
	int postMessage(string ref, int event, void *data);
	void notify(string ref, int notification, void *data);
	
	bool addEDL(string path, OEIdMap deviceIdMap);
	bool removeDevice(string deviceName);
	
private:
	void *oepa;
	
	void lock();
	void unlock();
};

#endif
