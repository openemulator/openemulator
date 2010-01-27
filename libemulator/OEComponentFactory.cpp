
/**
 * libemulator
 * Component Factory
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Builds components
 */

#include "OEComponentFactory.h"

// AUTOBUILD_INCLUDE_START - Do not modify this line
#include "EventController.h"
// AUTOBUILD_INCLUDE_END - Do not modify this line

#define buildComponent(name) if (className == "name") return new name()

OEComponent *OEComponentFactory::build(string className)
{
// AUTOBUILD_BUILD_START - Do not modify this line

	buildComponent(EventController);

// AUTOBUILD_BUILD_END - Do not modify this line
	
	return new OEComponent();
//	return NULL;
}
