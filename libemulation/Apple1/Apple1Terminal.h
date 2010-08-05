
/**
 * libemulation
 * Apple I Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Video
 */

#include "OEComponent.h"

#define APPLE1VIDEO_TERM_WIDTH 40
#define APPLE1VIDEO_TERM_HEIGHT 24

class Apple1Terminal : public Terminal
{
public:
	Apple1Terminal();
	
	bool setResource(const string &name, const OEData *data);
	bool connect(const string &name, OEComponent *component);
	
	void write(int address, int value);
	
private:
	OEComponent *host;
	OEComponent *system;
	OEComponent *pia;
	OEComponent *monitor;
	OEComponent *videoROM;
	
	OEData characterSet;
	char screen[APPLE1VIDEO_TERM_HEIGHT][APPLE1VIDEO_TERM_WIDTH];
};
