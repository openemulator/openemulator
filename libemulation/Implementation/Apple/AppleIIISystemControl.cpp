
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
#include "AppleIIIInterface.h"

#include "MOS6522.h"

AppleIIISystemControl::AppleIIISystemControl()
{
    cpu = NULL;
    controlBus = NULL;
    memoryBus = NULL;
    extendedMemoryBus = NULL;
    memoryAddressDecoder = NULL;
    memory = NULL;
    dac = NULL;
    
    zeroPage = 0;
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
    else if (name == "dac")
        dac = ref;
    else
        return false;
    
    return true;
}

bool AppleIIISystemControl::init()
{
    if (!cpu)
    {
        logMessage("cpu not connected");
        
        return false;
    }
    
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    if (!memoryBus)
    {
        logMessage("memoryBus not connected");
        
        return false;
    }
    
    if (!extendedMemoryBus)
    {
        logMessage("extendedMemoryBus not connected");
        
        return false;
    }
    
    if (!memoryAddressDecoder)
    {
        logMessage("memoryAddressDecoder not connected");
        
        return false;
    }
    
    if (!memory)
    {
        logMessage("memory not connected");
        
        return false;
    }
    
    if (!dac)
    {
        logMessage("dac not connected");
        
        return false;
    }
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x0000;
    offsetMap.endAddress = 0x1fff;
    offsetMap.offset = 0x8000 * APPLEIII_SYSTEMBANK - 0x0000;
    
    memory->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    
    offsetMap.startAddress = 0xa000;
    offsetMap.endAddress = 0xffff;
    offsetMap.offset = 0x8000 * APPLEIII_SYSTEMBANK - 0x8000;
    
    memory->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    
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
    environment = value;
    
    bool romSel1 = OEGetBit(environment, (1 << 0));
    bool ramWP = OEGetBit(environment, (1 << 3));
    bool resetEnabled = OEGetBit(environment, (1 << 4));
    bool videoEnabled = OEGetBit(environment, (1 << 5));
    bool ioEnabled = OEGetBit(environment, (1 << 6));
    bool slowSpeed = OEGetBit(environment, (1 << 7));
    
    OEInt memoryControl = 0;
    
    OESetBit(memoryControl, APPLEIII_RAMWP, ramWP);
    OESetBit(memoryControl, APPLEIII_IO, ioEnabled);
    OESetBit(memoryControl, APPLEIII_ROM, romSel1);
    
    memoryAddressDecoder->postMessage(this, APPLEIII_SET_MEMORYCONTROL, &memoryControl);
    
    updateNormalStack();
    
    float clockCPUMultiplier = slowSpeed ? 1 : 2;
    
    controlBus->postMessage(this, CONTROLBUS_SET_CPUCLOCKMULTIPLIER, &clockCPUMultiplier);
}

inline void AppleIIISystemControl::setZeroPage(OEChar value)
{
    zeroPage = value;
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x0000;
    offsetMap.endAddress = 0x00ff;
    offsetMap.offset = 0x100 * zeroPage;
    
    memoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    
    updateNormalStack();
    
    cpu->postMessage(this, APPLEIII_SET_ZEROPAGE, &zeroPage);
}

inline void AppleIIISystemControl::setRAMBank(OEChar value)
{
    OEChar bank = value & 0xf;
    
    if (bank == APPLEIII_SYSTEMBANK)
        bank = 0xf;
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x2000;
    offsetMap.endAddress = 0x9fff;
    offsetMap.offset = 0x8000 * bank - 0x2000;
    
    memory->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    
    bool appleIIMode = OEGetBit(value, (1 << 6));
    
    memoryAddressDecoder->postMessage(this, APPLEIII_SET_APPLEIIMODE, &appleIIMode);
}

inline void AppleIIISystemControl::setDACOutput(OEChar value)
{
    OEChar audioSample = value << 2;
    // bool BL = OEGetBit(value, (1 << 6));
    // bool ioNoNMI = OEGetBit(value, (1 << 7));
    
    dac->write(0, audioSample);
    dac->write(1, audioSample);
}

void AppleIIISystemControl::updateNormalStack()
{
    bool normalStack = OEGetBit(environment, (1 << 2));
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x0100;
    offsetMap.endAddress = 0x01ff;
    offsetMap.offset = normalStack ? 0 : (0x100 * (zeroPage ^ 0x01) - 0x100);
    
    memoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
}
