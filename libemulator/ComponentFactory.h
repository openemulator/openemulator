
/**
 * libemulator
 * Component Factory
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _COMPONENTFACTORY_H
#define _COMPONENTFACTORY_H

#include "Component.h"

class ComponentFactory
{
public:
	static Component *build(string className);
};

#endif
