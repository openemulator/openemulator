
/**
 * libemulation
 * Component Factory
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
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
	static OEComponent *construct(const string& className);
};

#endif
