
/**
 * libemulation
 * Component Factory
 * (C) 2009-2011 by Marc S. Ressl (mressl@umich.edu)
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
#include "ControlBus.h"
#include "FloatingBus.h"
#include "Monitor.h"
#include "RAM.h"
#include "ROM.h"
#include "Terminal.h"

#include "AppleDiskII.h"
#include "AppleDiskIIInterfaceCard.h"
#include "AppleGraphicsTablet.h"
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
#include "AppleIISuperSerialCard.h"
#include "AppleLanguageCard.h"
#include "AppleSilentype.h"

#include "MC6821.h"
#include "MC6845.h"

#include "MOS6502.h"
#include "MOS6530.h"
#include "MOSKIM1IO.h"
#include "MOSKIM1PLL.h"

#include "VidexVideoterm.h"
// FACTORY_INCLUDE_END - Do not modify this section

#define matchComponent(name) if (className == #name) return new name()

OEComponent *OEComponentFactory::construct(const string& className)
{
// FACTORY_CODE_START - Do not modify this section
	matchComponent(AddressDecoder);
	matchComponent(AddressOffset);
	matchComponent(AudioSampleConverter);
	matchComponent(CharsetLoader);
	matchComponent(ControlBus);
	matchComponent(FloatingBus);
	matchComponent(Monitor);
	matchComponent(RAM);
	matchComponent(ROM);
	matchComponent(Terminal);
	
	matchComponent(AppleDiskII);
	matchComponent(AppleDiskIIInterfaceCard);
	matchComponent(AppleGraphicsTablet);
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
	matchComponent(AppleIISuperSerialCard);
	matchComponent(AppleLanguageCard);
	matchComponent(AppleSilentype);
	
	matchComponent(MC6821);
	matchComponent(MC6845);
	
	matchComponent(MOS6502);
	matchComponent(MOS6530);
	matchComponent(MOSKIM1IO);
	matchComponent(MOSKIM1PLL);
    
    matchComponent(VidexVideoterm);
// FACTORY_CODE_END - Do not modify this section
	
	return NULL;
}
