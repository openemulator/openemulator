
/**
 * libemulation
 * Terminal
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a terminal
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "OEComponent.h"

class Terminal : public OEComponent
{
public:
	bool setValue(string name, string value);
	bool setRef(string name, OEComponent *ref);
	
private:
	int width;
	int height;
	OEComponent *charset;
};

#endif
