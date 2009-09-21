
/**
 * libemulator
 * Component Factory
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include "ComponentFactory.h"

#include "EventController.h"

#define buildComponent(name) if (className == "name") return new name()

Component *ComponentFactory::build(string className)
{
	buildComponent(EventController);
	
	return NULL;
}
