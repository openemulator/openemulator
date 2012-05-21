
/**
 * libemulation
 * Component Factory
 * (C) 2009-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Builds components
 */

#include "OEComponentFactory.h"

// FACTORY_INCLUDE_START - Do not modify this section
#include "AddressDecoder.h"
#include "AddressMapper.h"
#include "AddressMux.h"
#include "AddressOffset.h"
#include "AudioCodec.h"
#include "AudioPlayer.h"
#include "BankSwitchedRAM.h"
#include "ControlBus.h"
#include "FloatingBus.h"
#include "JoystickMapper.h"
#include "Monitor.h"
#include "RAM.h"
#include "ROM.h"

#include "Apple1ACI.h"
#include "Apple1IO.h"
#include "Apple1Terminal.h"
#include "AppleDiskDrive525.h"
#include "AppleDiskIIInterfaceCard.h"
#include "AppleGraphicsTablet.h"
#include "AppleLanguageCard.h"
#include "AppleIIAddressDecoder.h"
#include "AppleIIAudioIn.h"
#include "AppleIIAudioOut.h"
#include "AppleIIFloatingBus.h"
#include "AppleIIGamePort.h"
#include "AppleIIKeyboard.h"
#include "AppleIISlotMemory.h"
#include "AppleIIVideo.h"
#include "AppleSilentype.h"

#include "MOS6502.h"
#include "MOS6530.h"
#include "MOSKIM1IO.h"
#include "MOSKIM1PLL.h"

#include "MC6821.h"

#include "RDCFFA.h"
#include "RDCFFA1.h"

#include "VidexVideoterm.h"

#include "W65C02S.h"
// FACTORY_INCLUDE_END - Do not modify this section

#define matchComponent(name) if (className == #name) return new name()

OEComponent *OEComponentFactory::construct(const string& className)
{
    // FACTORY_CODE_START - Do not modify this section
    matchComponent(AddressDecoder);
    matchComponent(AddressMapper);
    matchComponent(AddressMux);
    matchComponent(AddressOffset);
    matchComponent(AudioCodec);
    matchComponent(AudioPlayer);
    matchComponent(BankSwitchedRAM);
    matchComponent(ControlBus);
    matchComponent(FloatingBus);
    matchComponent(JoystickMapper);
    matchComponent(Monitor);
    matchComponent(RAM);
    matchComponent(ROM);
    
    matchComponent(Apple1ACI);
    matchComponent(Apple1IO);
    matchComponent(Apple1Terminal);
    
    matchComponent(AppleDiskDrive525);
    matchComponent(AppleDiskIIInterfaceCard);
    matchComponent(AppleGraphicsTablet);
    matchComponent(AppleLanguageCard);
    matchComponent(AppleIIAddressDecoder);
    matchComponent(AppleIIAudioIn);
    matchComponent(AppleIIAudioOut);
    matchComponent(AppleIIFloatingBus);
    matchComponent(AppleIIGamePort);
    matchComponent(AppleIIKeyboard);
    matchComponent(AppleIISlotMemory);
    matchComponent(AppleIIVideo);
    matchComponent(AppleSilentype);
    
    matchComponent(MOS6502);
    matchComponent(MOS6530);
    matchComponent(MOSKIM1IO);
    matchComponent(MOSKIM1PLL);
    
    matchComponent(MC6821);
    
    matchComponent(RDCFFA);
    matchComponent(RDCFFA1);
    
    matchComponent(VidexVideoterm);
    
    matchComponent(W65C02S);
    // FACTORY_CODE_END - Do not modify this section
    
    return NULL;
}
