
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
#include "HostAudio.h"
#include "HostJoystick.h"
#include "HostKeyboard.h"
#include "HostSystem.h"
#include "HostVideo.h"
#include "MC6821.h"
// AUTOBUILD_INCLUDE_END - Do not modify this section

#define matchComponent(name) if (className == "name") return new name()

OEComponent *OEComponentFactory::build(string className)
{
// AUTOBUILD_CODE_START - Do not modify this section
	matchComponent(HostAudio);
	matchComponent(HostJoystick);
	matchComponent(HostKeyboard);
	matchComponent(HostSystem);
	matchComponent(HostVideo);
	matchComponent(MC6821);
// AUTOBUILD_CODE_END - Do not modify this section
	
	return new OEComponent();
//	return NULL;
}
