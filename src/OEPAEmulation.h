
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
	OEPAEmulation(OEPA *oepa, string path, string resourcePath);
	
	bool save(string path);
	
	bool setProperty(string ref, string name, string value);
	bool getProperty(string ref, string name, string &value);
	void postNotification(string ref, int notification, void *data);
	int ioctl(string ref, int message, void *data);
	
	bool addDML(string path, OEConnections connections);
	bool removeDevice(string ref);
	
private:
	void *oepa;
	
	void lock();
	void unlock();
};

#endif
