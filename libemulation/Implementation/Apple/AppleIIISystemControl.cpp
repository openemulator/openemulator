
/**
 * libemulation
 * Apple III System Control
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III system control
 */

#include "AppleIIISystemControl.h"

#include "ControlBusInterface.h"
#include "MemoryInterface.h"
#include "AppleIIInterface.h"

#include "MOS6522.h"

AppleIIISystemControl::AppleIIISystemControl()
{
    cpu = NULL;
    controlBus = NULL;
    memoryBus = NULL;
    extendedMemoryBus = NULL;
    memoryAddressDecoder = NULL;
    memory = NULL;
    video = NULL;
    dVIA = NULL;
    eVIA = NULL;
    dac = NULL;
    
    zeroPage = 0;
    environment = 0;
    ramBank = 0;
    dacOutput = 0;
    
    monitorRequested = false;
}

bool AppleIIISystemControl::setRef(string name, OEComponent *ref)
{
    if (name == "cpu")
        cpu = ref;
    else if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
    }
    else if (name == "memoryBus")
        memoryBus = ref;
    else if (name == "extendedMemoryBus")
        extendedMemoryBus = ref;
    else if (name == "memoryAddressDecoder")
        memoryAddressDecoder = ref;
    else if (name == "memory")
        memory = ref;
    else if (name == "video")
        video = ref;
    else if (name == "dVIA")
        dVIA = ref;
    else if (name == "eVIA")
        eVIA = ref;
    else if (name == "dac")
        dac = ref;
    else
        return false;
    
    return true;
}

bool AppleIIISystemControl::init()
{
    OECheckComponent(cpu);
    OECheckComponent(controlBus);
    OECheckComponent(memoryBus);
    OECheckComponent(extendedMemoryBus);
    OECheckComponent(memoryAddressDecoder);
    OECheckComponent(memory);
    OECheckComponent(video);
    OECheckComponent(dVIA);
    OECheckComponent(eVIA);
    OECheckComponent(dac);
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x0000;
    offsetMap.endAddress = 0x1fff;
    offsetMap.offset = 0x8000 * APPLEIII_SYSTEMBANK - 0x0000;
    
    memory->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    
    offsetMap.startAddress = 0xa000;
    offsetMap.endAddress = 0xffff;
    offsetMap.offset = 0x8000 * APPLEIII_SYSTEMBANK - 0x8000;
    
    memory->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    
    dVIA->postMessage(this, MOS6522_GET_PA, &environment);
    dVIA->postMessage(this, MOS6522_GET_PB, &zeroPage);
    eVIA->postMessage(this, MOS6522_GET_PB, &ramBank);
    eVIA->postMessage(this, MOS6522_GET_PB, &dacOutput);
    
    return true;
}

void AppleIIISystemControl::notify(OEComponent *sender, int notification, void *data)
{
    setEnvironment(0xff);
    setZeroPage(0);
    setRAMBank(0);
    setDACOutput(0);
}

OEChar AppleIIISystemControl::read(OEAddress address)
{
//    logMessage("R " + getString(address));
    
    switch (address)
    {
        case 0x0:
            return 0;
            
        case 0x1:
            return 0;
            
        case 0x2:
            return 0xb0;    // For now, return IRQ's clear
            
        case 0x3:
            return 0;
    }
    
    return 0;
}

void AppleIIISystemControl::write(OEAddress address, OEChar value)
{
    switch (address)
    {
        case 0x0:
            setEnvironment(value);
            
            break;
            
        case 0x1:
            setZeroPage(value);
            
            break;
            
        case 0x2:
            setRAMBank(value);
            
            break;
            
        case 0x3:
            setDACOutput(value);
            
            break;
    }
    
//    logMessage("W " + getString(address) + ":" + getHexString(value));
}

inline void AppleIIISystemControl::setEnvironment(OEChar value)
{
    if (environment != value)
    {
        environment = value;
        
        /*    if (!videoEnabled && newVideoEnabled)
         video->postMessage(this, APPLEII_RELEASE_MONITOR, NULL);
         else if (videoEnabled && !newVideoEnabled)
         video->postMessage(this, APPLEII_REQUEST_MONITOR, NULL);*/
        
        float clockCPUMultiplier = OEGetBit(value, APPLEIII_SLOWSPEED) ? 1 : 2;
        
        controlBus->postMessage(this, CONTROLBUS_SET_CPUCLOCKMULTIPLIER, &clockCPUMultiplier);
        
        postNotification(this, APPLEIII_ENVIRONMENT_DID_CHANGE, &value);
        
        updateNormalStack();
    }
}

inline void AppleIIISystemControl::setZeroPage(OEChar value)
{
    if (zeroPage != value)
    {
        zeroPage = value;
        
        AddressOffsetMap offsetMap;
        
        offsetMap.startAddress = 0x0000;
        offsetMap.endAddress = 0x00ff;
        offsetMap.offset = 0x100 * zeroPage;
        
        memoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
        extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
        
        cpu->postMessage(this, APPLEIII_SET_ZEROPAGE, &zeroPage);
        
        updateNormalStack();
    }
}

inline void AppleIIISystemControl::setRAMBank(OEChar value)
{
    bool appleIIMode = !OEGetBit(value, APPLEIII_NOTAPPLEIIMODE);
    
    if (appleIIMode != !OEGetBit(ramBank, APPLEIII_NOTAPPLEIIMODE))
        postNotification(this, APPLEIII_APPLEIIMODE_DID_CHANGE, &appleIIMode);
    
    if ((value & 0xf) != (ramBank & 0xf))
    {
        ramBank = value;
        
        OEChar bank = ramBank & 0xf;
        
        if (bank == APPLEIII_SYSTEMBANK)
            bank = 0xf;
        
        AddressOffsetMap offsetMap;
        
        offsetMap.startAddress = 0x2000;
        offsetMap.endAddress = 0x9fff;
        offsetMap.offset = 0x8000 * bank - 0x2000;
        
        memory->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    }
    
    ramBank = value;
}

inline void AppleIIISystemControl::setDACOutput(OEChar value)
{
    if (dacOutput == value)
        return;
    
    OEChar audioSample = value << 2;
    // bool BL = OEGetBit(value, (1 << 6));
    // bool ioNoNMI = OEGetBit(value, (1 << 7));
    
    dac->write(0, audioSample);
    dac->write(1, audioSample);
    
    dacOutput = value;
}

void AppleIIISystemControl::updateNormalStack()
{
    bool normalStack = OEGetBit(environment, APPLEIII_NORMALSTACK);
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x0100;
    offsetMap.endAddress = 0x01ff;
    offsetMap.offset = normalStack ? 0 : (0x100 * (zeroPage ^ 0x01) - 0x100);
    
    memoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
}
