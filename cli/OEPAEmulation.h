
/**
 * OpenEmulator
 * OEEmulation portaudio class
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OEEmulation OEPA enhanced interface.
 */

#ifndef _OEPAEMULATION_H
#define _OEPAEMULATION_H

#include "OEEmulation.h"

typedef struct OEPA;

class OEPAEmulation : public OEEmulation
{
public:
	OEPAEmulation(OEPA *oepa, string path, string resourcePath);
	
	bool save(string path);
	
	bool setProperty(string ref, string name, string value);
	bool getProperty(string ref, string name, string &value);
	void postNotification(string ref, int notification, void *data);
	int ioctl(string ref, int message, void *data);
	
	bool addDevices(string path, OEStringRefMap connections);
	bool isDeviceTerminal(OERef ref);
	bool removeDevice(OERef ref);
	
private:
	OEPA *oepa;
};

#endif
