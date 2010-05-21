
/**
 * libemulator
 * Component Factory
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Builds components
 */

#include "OEComponentFactory.h"

// FACTORY_INCLUDE_START - Do not modify this section
#include "HostSystem.h"
#include "HostVideo.h"
#include "HostAudio.h"
#include "HostKeyboard.h"
#include "HostJoystick.h"
#include "HostMouse.h"
#include "HostTablet.h"
#include "HostCamera.h"
#include "HostParallel.h"
#include "HostSerial.h"
#include "HostNet.h"

#include "RAM.h"
#include "ROM.h"
#include "MemoryMap8Bit.h"
#include "MemoryMap16Bit.h"
#include "MemoryOffset.h"
#include "CompositeMonitor.h"
#include "DAC1bit.h"
#include "MC6821.h"
#include "MC6845.h"

#include "MOS6502.h"

#include "AppleISystem.h"
#include "AppleIFloatingBus.h"
#include "AppleIVideo.h"
#include "AppleIKeyboard.h"
#include "AppleIIO.h"
#include "AppleICassetteInterface.h"

// FACTORY_INCLUDE_END - Do not modify this section

#define matchComponent(name) if (className == #name) return new name()

OEComponent *OEComponentFactory::build(string className)
{
// FACTORY_CODE_START - Do not modify this section
	matchComponent(HostSystem);
	matchComponent(HostVideo);
	matchComponent(HostAudio);
	matchComponent(HostKeyboard);
	matchComponent(HostJoystick);
	matchComponent(HostMouse);
	matchComponent(HostTablet);
	matchComponent(HostCamera);
	matchComponent(HostParallel);
	matchComponent(HostSerial);
	matchComponent(HostNet);
	
	matchComponent(RAM);
	matchComponent(ROM);
	matchComponent(MemoryMap8Bit);
	matchComponent(MemoryMap16Bit);
	matchComponent(MemoryOffset);
	matchComponent(CompositeMonitor);
	matchComponent(DAC1Bit);
	matchComponent(MC6821);
	matchComponent(MC6845);
	
	matchComponent(MOS6502);
	
	matchComponent(AppleISystem);
	matchComponent(AppleIFloatingBus);
	matchComponent(AppleIVideo);
	matchComponent(AppleIKeyboard);
	matchComponent(AppleIIO);
// FACTORY_CODE_END - Do not modify this section
	
	return NULL;
}
