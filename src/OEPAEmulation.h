
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
	OEPAEmulation(string path, string resourcePath);
	OEPAEmulation(OEPA *oepa, string path, string resourcePath);
	
	bool open(string path);
	bool save(string path);
	
	bool setProperty(string ref, string name, string value);
	bool getProperty(string ref, string name, string &value);
	
	void notify(string ref, int notification, void *data);
	
	int postEvent(string ref, int message, void *data);
	
	bool addDML(string path, OEConnections &connections);
	bool removeDevice(string deviceName);
	
private:
	void *oepa;
	
	void lock();
	void unlock();
};

#endif
