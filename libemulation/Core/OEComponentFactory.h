
/**
 * libemulation
 * Component Factory
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Builds components
 */

#ifndef _OECOMPONENTFACTORY_H
#define _OECOMPONENTFACTORY_H

#include "OEComponent.h"

class OEComponentFactory
{
public:
	static OEComponent *create(const string &className);
};

#endif
