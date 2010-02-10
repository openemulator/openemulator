
/**
 * libemulator
 * Component Factory
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Builds components
 */

#include "OEComponentFactory.h"

// AUTOBUILD_INCLUDE_START - Do not modify this section
#include "EventController.h"
// AUTOBUILD_INCLUDE_END - Do not modify this section

#define buildComponent(name) if (className == "name") return new name()

OEComponent *OEComponentFactory::build(string className)
{
// AUTOBUILD_CODE_START - Do not modify this section
	buildComponent(EventController);
// AUTOBUILD_CODE_END - Do not modify this section
	
	return new OEComponent();
//	return NULL;
}
