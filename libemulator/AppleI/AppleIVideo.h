
/**
 * libemulator
 * Apple I Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Video
 */

#include "OEComponent.h"

#define APPLEIVIDEO_TERM_WIDTH 40
#define APPLEIVIDEO_TERM_HEIGHT 24

class AppleIVideo : public OEComponent
{
public:
	AppleIVideo();
	
	bool setResource(const string &name, const OEData &data);
	bool connect(const string &name, OEComponent *component);
	
	void write(int address, int value);
	
private:
	OEComponent *cpu;
	OEComponent *cpuSocket;
	
	vector<char> characterSet;
	char screen[APPLEIVIDEO_TERM_HEIGHT][APPLEIVIDEO_TERM_WIDTH];
};
