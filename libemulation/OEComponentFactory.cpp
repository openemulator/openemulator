
/**
 * libemulation
 * Component Factory
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Builds components
 */

#include "OEComponentFactory.h"

// FACTORY_INCLUDE_START - Do not modify this section
#include "Host.h"
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
	matchComponent(Host);
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
	matchComponent(AppleICassetteInterface);
// FACTORY_CODE_END - Do not modify this section
	
	return NULL;
}
