
/**
 * libemulation
 * Apple I Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Video
 */

#include "Terminal.h"

#define APPLE1TERMINAL_DEFAULT_WIDTH 40
#define APPLE1TERMINAL_DEFAULT_HEIGHT 24

class Apple1Terminal : public Terminal
{
public:
	Apple1Terminal();
	
	bool setRef(string name, OEComponent *id);
	
	void write(int address, int value);
	
private:
	OEComponent *host;
	OEComponent *charset;
	OEComponent *monitor;
	
	OEData characterSet;
	char screen[APPLE1TERMINAL_DEFAULT_HEIGHT][APPLE1TERMINAL_DEFAULT_WIDTH];
};
