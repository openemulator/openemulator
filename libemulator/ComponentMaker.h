
/**
 * libappleiigo
 * Component Maker
 * (C) 2008 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef COMPONENTMAKER_H
#define COMPONENTMAKER_H

#include <string>

#include "Component.h"

class ComponentMaker {
public:
	static class Component * getComponent(string &name);
};

#endif
