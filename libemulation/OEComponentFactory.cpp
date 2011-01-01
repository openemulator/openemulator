
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
#include "AddressDecoder.h"
#include "AddressOffset.h"
#include "AudioSampleConverter.h"
#include "CharsetLoader.h"
#include "CompositeMonitor.h"
#include "ControlBus.h"
#include "FloatingBus.h"
#include "RAM.h"
#include "ROM.h"
#include "Terminal.h"

#include "Apple1IO.h"
#include "Apple1Terminal.h"
#include "Apple1CassetteInterfaceCard.h"
#include "AppleIIControlBus.h"
#include "AppleIIMMU.h"
#include "AppleIIFloatingBus.h"
#include "AppleIIKeyboard.h"
#include "AppleIIVideo.h"
#include "AppleIIAudioOut.h"
#include "AppleIIAudioIn.h"
#include "AppleIIGamePort.h"
#include "AppleIISlotMemory.h"
#include "AppleIISlotExpansionMemory.h"
#include "AppleDiskII.h"

#include "MC6821.h"
#include "MC6845.h"

#include "MOS6502.h"
#include "MOS6530.h"
#include "MOSKIM1IO.h"
#include "MOSKIM1PLL.h"
// FACTORY_INCLUDE_END - Do not modify this section

#define matchComponent(name) if (className == #name) return new name()

OEComponent *OEComponentFactory::create(const string &className)
{
// FACTORY_CODE_START - Do not modify this section
	matchComponent(AddressDecoder);
	matchComponent(AddressOffset);
	matchComponent(AudioSampleConverter);
	matchComponent(CharsetLoader);
	matchComponent(CompositeMonitor);
	matchComponent(ControlBus);
	matchComponent(FloatingBus);
	matchComponent(RAM);
	matchComponent(ROM);
	matchComponent(Terminal);
	
	matchComponent(Apple1IO);
	matchComponent(Apple1Terminal);
	matchComponent(Apple1CassetteInterfaceCard);
	matchComponent(AppleIIControlBus);
	matchComponent(AppleIIMMU);
	matchComponent(AppleIIFloatingBus);
	matchComponent(AppleIIKeyboard);
	matchComponent(AppleIIVideo);
	matchComponent(AppleIIAudioOut);
	matchComponent(AppleIIAudioIn);
	matchComponent(AppleIIGamePort);
	matchComponent(AppleIISlotMemory);
	matchComponent(AppleIISlotExpansionMemory);
	matchComponent(AppleDiskII);
	
	matchComponent(MC6821);
	matchComponent(MC6845);
	
	matchComponent(MOS6502);
	matchComponent(MOS6530);
	matchComponent(MOSKIM1IO);
	matchComponent(MOSKIM1PLL);
// FACTORY_CODE_END - Do not modify this section
	
	return NULL;
}
