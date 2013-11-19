
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
#include "AddressMasker.h"
#include "AddressMux.h"
#include "AddressOffset.h"
#include "ATAController.h"
#include "ATADevice.h"
#include "AudioCodec.h"
#include "AudioPlayer.h"
#include "ControlBus.h"
#include "FloatingBus.h"
#include "JoystickMapper.h"
#include "Monitor.h"
#include "Proxy.h"
#include "RAM.h"
#include "ROM.h"
#include "VRAM.h"

#include "Apple1ACI.h"
#include "Apple1IO.h"
#include "Apple1Terminal.h"
#include "AppleDiskDrive525.h"
#include "AppleDiskIIInterfaceCard.h"
#include "AppleGraphicsTablet.h"
#include "AppleGraphicsTabletInterfaceCard.h"
#include "AppleLanguageCard.h"
#include "AppleSilentype.h"
#include "AppleSilentypeInterfaceCard.h"

#include "AppleIIAddressDecoder.h"
#include "AppleIIAudioIn.h"
#include "AppleIIAudioOut.h"
#include "AppleIIDisableC800.h"
#include "AppleIIFloatingBus.h"
#include "AppleIIGamePort.h"
#include "AppleIIKeyboard.h"
#include "AppleIISlotController.h"
#include "AppleIIVideo.h"
#include "AppleIISystemControl.h"

#include "AppleIIIAddressDecoder.h"
#include "AppleIIIBeeper.h"
#include "AppleIIIDiskIO.h"
#include "AppleIIIGamePort.h"
#include "AppleIIIKeyboard.h"
#include "AppleIIIMOS6502.h"
#include "AppleIIIRTC.h"
#include "AppleIIISystemControl.h"
#include "AppleIIIVideo.h"

#include "MOS6502.h"
#include "MOS6522.h"
#include "MOS6530.h"
#include "MOS6551.h"
#include "MOSKIM1IO.h"
#include "MOSKIM1PLL.h"

#include "MC6821.h"

#include "MM58167.h"

#include "RDCFFA.h"

#include "VidexVideoterm.h"

#include "W65C02S.h"
// FACTORY_INCLUDE_END - Do not modify this section

#define matchComponent(name) if (className == #name) return new name()

OEComponent *OEComponentFactory::construct(const string& className)
{
    // FACTORY_CODE_START - Do not modify this section
    matchComponent(AddressDecoder);
    matchComponent(AddressMapper);
    matchComponent(AddressMasker);
    matchComponent(AddressMux);
    matchComponent(AddressOffset);
    matchComponent(ATAController);
    matchComponent(ATADevice);
    matchComponent(AudioCodec);
    matchComponent(AudioPlayer);
    matchComponent(ControlBus);
    matchComponent(FloatingBus);
    matchComponent(JoystickMapper);
    matchComponent(Monitor);
    matchComponent(Proxy);
    matchComponent(RAM);
    matchComponent(ROM);
    matchComponent(VRAM);
    
    matchComponent(Apple1ACI);
    matchComponent(Apple1IO);
    matchComponent(Apple1Terminal);
    
    matchComponent(AppleDiskDrive525);
    matchComponent(AppleDiskIIInterfaceCard);
    matchComponent(AppleGraphicsTablet);
    matchComponent(AppleGraphicsTabletInterfaceCard);
    matchComponent(AppleLanguageCard);
    matchComponent(AppleSilentype);
    matchComponent(AppleSilentypeInterfaceCard);
    
    matchComponent(AppleIIAddressDecoder);
    matchComponent(AppleIIAudioIn);
    matchComponent(AppleIIAudioOut);
    matchComponent(AppleIIDisableC800);
    matchComponent(AppleIIFloatingBus);
    matchComponent(AppleIIGamePort);
    matchComponent(AppleIIKeyboard);
    matchComponent(AppleIISlotController);
    matchComponent(AppleIIVideo);
    matchComponent(AppleIISystemControl);
    
    matchComponent(AppleIIIAddressDecoder);
    matchComponent(AppleIIIBeeper);
    matchComponent(AppleIIIDiskIO);
    matchComponent(AppleIIIGamePort);
    matchComponent(AppleIIIKeyboard);
    matchComponent(AppleIIIMOS6502);
    matchComponent(AppleIIIRTC);
    matchComponent(AppleIIISystemControl);
//    matchComponent(AppleIIIVideo);
    
    matchComponent(MOS6502);
    matchComponent(MOS6522);
    matchComponent(MOS6530);
    matchComponent(MOS6551);
    matchComponent(MOSKIM1IO);
    matchComponent(MOSKIM1PLL);
    
    matchComponent(MC6821);
    
    matchComponent(MM58167);
    
    matchComponent(RDCFFA);
    
    matchComponent(VidexVideoterm);
    
    matchComponent(W65C02S);
    // FACTORY_CODE_END - Do not modify this section
    
    return NULL;
}
