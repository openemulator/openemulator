
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

#define CPUSPEED_SLOW       1.0F
#define CPUSPEED_FAST       ((1.0F * 9 + 2.0F * (65 - 9)) / 65)
#define CPUSPEED_FAST_VIDEO ((CPUSPEED_FAST * (912 - 560) + CPUSPEED_SLOW * 560) / 912)

AppleIIISystemControl::AppleIIISystemControl()
{
    cpu = NULL;
    controlBus = NULL;
    memoryBus = NULL;
    addressDecoder = NULL;
    bankSwitcher = NULL;
    extendedMemoryBus = NULL;
    ram = NULL;
    video = NULL;
    dVIA = NULL;
    eVIA = NULL;
    dac = NULL;
    
    zeroPage = 0;
    environment = 0;
    ramBank = 0;
    sound = 0;
    
    extendedRAMBank = 0;
    
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
    else if (name == "addressDecoder")
        addressDecoder = ref;
    else if (name == "bankSwitcher")
        bankSwitcher = ref;
    else if (name == "extendedMemoryBus")
        extendedMemoryBus = ref;
    else if (name == "extendedBankSwitcher")
        extendedBankSwitcher = ref;
    else if (name == "ram")
    {
        if (ram)
            ram->removeObserver(this, RAM_SIZE_DID_CHANGE);
        ram = ref;
        if (ram)
            ram->addObserver(this, RAM_SIZE_DID_CHANGE);
    }
    else if (name == "video")
    {
        if (video)
        {
            video->removeObserver(this, APPLEII_VBL_DID_BEGIN);
            video->removeObserver(this, APPLEII_VBL_DID_END);
        }
        video = ref;
        if (video)
        {
            video->addObserver(this, APPLEII_VBL_DID_BEGIN);
            video->addObserver(this, APPLEII_VBL_DID_END);
        }
    }
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
    OECheckComponent(addressDecoder);
    OECheckComponent(bankSwitcher);
    OECheckComponent(extendedMemoryBus);
    OECheckComponent(extendedBankSwitcher);
    OECheckComponent(ram);
    OECheckComponent(video);
    OECheckComponent(dVIA);
    OECheckComponent(eVIA);
    OECheckComponent(dac);
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x0000;
    offsetMap.endAddress = 0x1fff;
    offsetMap.offset = 0x8000 * APPLEIII_SYSTEMBANK - 0x0000;
    
    bankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    
    offsetMap.startAddress = 0xa000;
    offsetMap.endAddress = 0xffff;
    offsetMap.offset = 0x8000 * APPLEIII_SYSTEMBANK - 0x8000;
    
    bankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    
    dVIA->postMessage(this, MOS6522_GET_PA, &environment);
    dVIA->postMessage(this, MOS6522_GET_PB, &zeroPage);
    eVIA->postMessage(this, MOS6522_GET_PA, &ramBank);
    eVIA->postMessage(this, MOS6522_GET_PB, &sound);
    
    updateEnvironment();
    updateZeroPage();
    updateRAMBank();
    
    updateExtendedRAMBank();
    
    // Get RAM Size
    
    return true;
}

bool AppleIIISystemControl::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case APPLEIII_GET_ENVIRONMENT:
            *((OEChar *)data) = environment;
            
            return true;
            
        case APPLEIII_GET_APPLEIIMODE:
            *((bool *)data) = !OEGetBit(ramBank, APPLEIII_NOT_APPLEIIMODE);
            
            return true;
            
        case APPLEIII_GET_NOT_IONMI:
            *((bool *)data) = OEGetBit(sound, APPLEIII_NOT_IONMI);
            
            return true;
            
        case APPLEIII_SET_EXTENDEDRAMBANK:
            setExtendedRAMBank(*((OEChar *)data));
            
            return true;
            
        default:
            return false;
    }
}

void AppleIIISystemControl::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        logMessage("AppleIIISystemControl reset");
        
/*        setEnvironment(0);
        setZeroPage(0);
        setRAMBank(0);
        setSound(0);*/
    }
    else if (sender == video)
    {
        bool videoEnabled = OEGetBit(environment, APPLEIII_VIDEOENABLE);
        bool slowSpeed = OEGetBit(environment, APPLEIII_SLOWSPEED);
        
        if (videoEnabled && !slowSpeed)
        {
            float clockCPUMultiplier;
            
            if (notification == APPLEII_VBL_DID_END)
                clockCPUMultiplier = CPUSPEED_FAST_VIDEO;
            else
                clockCPUMultiplier = CPUSPEED_FAST;
            
            controlBus->postMessage(this, CONTROLBUS_SET_CPUCLOCKMULTIPLIER, &clockCPUMultiplier);
        }
    }
    else if (sender == ram)
    {
        // To-Do: Update
    }
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
        {
            bool vbl;
            
            video->postMessage(this, APPLEII_IS_VBL, &vbl);
            
            OEChar value = 0;
            
            OESetBit(value, APPLEIII_BL, vbl);
            
            return value;
        }
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
            setSound(value);
            
            break;
    }
    
//    logMessage("W " + getString(address) + ":" + getHexString(value));
}

void AppleIIISystemControl::setEnvironment(OEChar value)
{
    if (environment == value)
        return;
    
    environment = value;
    
    postNotification(this, APPLEIII_ENVIRONMENT_DID_CHANGE, &value);
    
    updateEnvironment();
}

void AppleIIISystemControl::setZeroPage(OEChar value)
{
    if (zeroPage == value)
        return;
    
    zeroPage = value;
    
    updateZeroPage();
}

void AppleIIISystemControl::setRAMBank(OEChar value)
{
    logMessage("ramBank: " + getBinString(value, 8));
    
    if (OEGetBit(ramBank, APPLEIII_NOT_APPLEIIMODE) !=
        OEGetBit(value, APPLEIII_NOT_APPLEIIMODE))
    {
        OESetBit(ramBank, APPLEIII_NOT_APPLEIIMODE, OEGetBit(value, APPLEIII_NOT_APPLEIIMODE));
        
        bool appleIIMode = !OEGetBit(ramBank, APPLEIII_NOT_APPLEIIMODE);
        
        postNotification(this, APPLEIII_APPLEIIMODE_DID_CHANGE, &appleIIMode);
    }
    
    if ((ramBank & 0xf) != (value & 0xf))
    {
        ramBank = (ramBank & 0xf0) | (value & 0xf);
        
        updateRAMBank();
    }
}

void AppleIIISystemControl::setExtendedRAMBank(OEChar value)
{
    if (extendedRAMBank == value)
        return;
    
    extendedRAMBank = value;
    
    updateExtendedRAMBank();
}

void AppleIIISystemControl::setSound(OEChar value)
{
    OEChar audioSample = 4 * value;
    // bool ioNoNMI = OEGetBit(value, (1 << 7));
    
    dac->write(0, audioSample);
    dac->write(1, audioSample);
    
    sound = value;
}

void AppleIIISystemControl::updateEnvironment()
{
    logMessage("environment ro:" + getString(OEGetBit(environment, APPLEIII_ROMSEL1)) +
               " r2:" + getString(OEGetBit(environment, APPLEIII_ROMSEL2)) +
               " ns:" + getString(OEGetBit(environment, APPLEIII_NORMALSTACK)) +
               " wp:" + getString(OEGetBit(environment, APPLEIII_RAMWP)) +
               " rs:" + getString(OEGetBit(environment, APPLEIII_RESETENABLE)) +
               " vi:" + getString(OEGetBit(environment, APPLEIII_VIDEOENABLE)) +
               " io:" + getString(OEGetBit(environment, APPLEIII_IOENABLE)) +
               " ss:" + getString(OEGetBit(environment, APPLEIII_SLOWSPEED))
               );
    
    /*    if (!videoEnabled && newVideoEnabled)
     video->postMessage(this, APPLEII_RELEASE_MONITOR, NULL);
     else if (videoEnabled && !newVideoEnabled)
     video->postMessage(this, APPLEII_REQUEST_MONITOR, NULL);*/
    
    bool videoEnabled = OEGetBit(environment, APPLEIII_VIDEOENABLE);
    bool slowSpeed = OEGetBit(environment, APPLEIII_SLOWSPEED);
    bool isVBL;
    
    video->postMessage(this, APPLEII_IS_VBL, &isVBL);
    
    float clockCPUMultiplier;
    
    if (slowSpeed)
        clockCPUMultiplier = CPUSPEED_SLOW;
    else if (videoEnabled && !isVBL)
        clockCPUMultiplier = CPUSPEED_FAST_VIDEO;
    else
        clockCPUMultiplier = CPUSPEED_FAST;
    
    controlBus->postMessage(this, CONTROLBUS_SET_CPUCLOCKMULTIPLIER, &clockCPUMultiplier);
    
    updateAltStack();
}

void AppleIIISystemControl::updateZeroPage()
{
    logMessage("zeroPage: " + getString(zeroPage));
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x0000;
    offsetMap.endAddress = 0x00ff;
    offsetMap.offset = 0x100 * zeroPage;
    
    memoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    
    cpu->postMessage(this, APPLEIII_SET_ZEROPAGE, &zeroPage);
    
    if (!OEGetBit(environment, APPLEIII_NORMALSTACK))
        updateAltStack();
}

void AppleIIISystemControl::updateAltStack()
{
    bool altStack = !OEGetBit(environment, APPLEIII_NORMALSTACK);
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x0100;
    offsetMap.endAddress = 0x01ff;
    offsetMap.offset = altStack ? (0x100 * (zeroPage ^ 0x01) - 0x100) : 0;
    
    memoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    extendedMemoryBus->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
}

void AppleIIISystemControl::updateRAMBank()
{
    OEChar bank = ramBank & 0x7;
    
    if (bank == APPLEIII_SYSTEMBANK)
        bank = 0x7;
    
    logMessage("ramBank: " + getString(bank));
    
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x2000;
    offsetMap.endAddress = 0x9fff;
    offsetMap.offset = 0x8000 * bank - 0x2000;
    
    bankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
}

void AppleIIISystemControl::updateExtendedRAMBank()
{
    logMessage("extendedRAMBank: " + getString(extendedRAMBank));
    
    AddressOffsetMap offsetMap;
    
    if (extendedRAMBank == 0xf)
    {
        offsetMap.startAddress = 0x0000;
        offsetMap.endAddress = 0x1fff;
        offsetMap.offset = APPLEIII_SYSTEMBANK * 0x8000 - 0x0000;
        
        extendedBankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
        
        offsetMap.startAddress = 0x2000;
        offsetMap.endAddress = 0x9fff;
        offsetMap.offset = 0x0000 - 0x2000;
        
        extendedBankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
        
        offsetMap.startAddress = 0xa000;
        offsetMap.endAddress = 0xffff;
        offsetMap.offset = APPLEIII_SYSTEMBANK * 0x8000 - 0x8000;
        
        extendedBankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    }
    else
    {
        OEChar bank = extendedRAMBank & 0x7;
        
        if (bank == APPLEIII_SYSTEMBANK)
            bank = 0x7;
        
        offsetMap.startAddress = 0x0000;
        offsetMap.endAddress = 0x7fff;
        offsetMap.offset = extendedRAMBank * 0x8000 - 0x0000;
        
        extendedBankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
        
        offsetMap.startAddress = 0x8000;
        offsetMap.endAddress = 0xffff;
        offsetMap.offset = (extendedRAMBank + 1) * 0x8000 - 0x8000;
        
        extendedBankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
    }
}
