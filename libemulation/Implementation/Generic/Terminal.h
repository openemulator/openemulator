
/**
 * libemulation
 * Terminal
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a terminal.
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "OEComponent.h"

class Terminal : public OEComponent
{
public:
	Terminal();
	
	bool setRef(string name, OEComponent *ref);
	
private:
	OEComponent *device;
    
	OEComponent *canvas;
};

#endif
