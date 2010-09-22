
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
#include "ControlBus.h"
#include "AddressDecoder.h"
#include "AddressOffset.h"
#include "RAM.h"
#include "ROM.h"
#include "CompositeMonitor.h"
#include "Audio1Bit.h"
#include "AudioPLL.h"

#include "MC6821.h"
#include "MC6845.h"

#include "MOS6502.h"
#include "MOS6530.h"

#include "KIM1IO.h"

#include "Apple1IO.h"
#include "Apple1Terminal.h"
#include "Apple1CassetteInterface.h"
// FACTORY_INCLUDE_END - Do not modify this section

#define matchComponent(name) if (className == #name) return new name()

OEComponent *OEComponentFactory::build(string className)
{
	// FACTORY_CODE_START - Do not modify this section
	matchComponent(Host);
	matchComponent(ControlBus);
	matchComponent(AddressDecoder);
	matchComponent(AddressOffset);
	matchComponent(RAM);
	matchComponent(ROM);
	matchComponent(CompositeMonitor);
	matchComponent(Audio1Bit);
	matchComponent(AudioPLL);
	
	matchComponent(MC6821);
	matchComponent(MC6845);
	
	matchComponent(MOS6502);
	matchComponent(MOS6530);
	
	matchComponent(KIM1IO);
	
	matchComponent(Apple1IO);
	matchComponent(Apple1Terminal);
	matchComponent(Apple1CassetteInterface);
	// FACTORY_CODE_END - Do not modify this section
	
	return NULL;
}
