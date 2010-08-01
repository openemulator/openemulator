
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
#include "MemoryMap.h"
#include "MemoryOffset.h"
#include "FloatingBus.h"
#include "CompositeMonitor.h"
#include "Audio1Bit.h"
#include "AudioPLL.h"

#include "MC6821.h"
#include "MC6845.h"

#include "MOS6502.h"
#include "MOS6530.h"

#include "Apple1System.h"
#include "Apple1FloatingBus.h"
#include "Apple1Video.h"
#include "Apple1Keyboard.h"
#include "Apple1IO.h"
#include "Apple1CassetteInterface.h"
// FACTORY_INCLUDE_END - Do not modify this section

#define matchComponent(name) if (className == #name) return new name()

OEComponent *OEComponentFactory::build(string className)
{
// FACTORY_CODE_START - Do not modify this section
	matchComponent(Host);
	matchComponent(RAM);
	matchComponent(ROM);
	matchComponent(MemoryMap);
	matchComponent(MemoryOffset);
	matchComponent(FloatingBus);
	matchComponent(CompositeMonitor);
	matchComponent(Audio1Bit);
	matchComponent(AudioPLL);
	
	matchComponent(MC6821);
	matchComponent(MC6845);
	
	matchComponent(MOS6502);
	matchComponent(MOS6530);
	
	matchComponent(Apple1System);
	matchComponent(Apple1FloatingBus);
	matchComponent(Apple1Video);
	matchComponent(Apple1Keyboard);
	matchComponent(Apple1IO);
	matchComponent(Apple1CassetteInterface);
// FACTORY_CODE_END - Do not modify this section
	
	return NULL;
}
