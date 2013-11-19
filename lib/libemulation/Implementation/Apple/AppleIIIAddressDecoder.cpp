
/**
 * libemulation
 * Apple III Address Decoder
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple III Address Decoder
 */

#include "AppleIIIAddressDecoder.h"

#include "AppleIIIInterface.h"

AppleIIIAddressDecoder::AppleIIIAddressDecoder() : AddressDecoder()
{
    bankSwitcher = NULL;
    io = NULL;
    rom = NULL;
    memoryFF00 = NULL;
    systemControl = NULL;
    
    for (OEInt i = 0; i < 8; i++)
        slot[i] = NULL;
    
    environment = 0xff;
    appleIIMode = false;
}

bool AppleIIIAddressDecoder::setRef(string name, OEComponent *ref)
{
    if (name == "bankSwitcher")
        bankSwitcher = ref;
    else if (name == "io")
        io = ref;
    else if (name == "slot1")
        setSlot(1, ref);
    else if (name == "slot2")
        setSlot(2, ref);
    else if (name == "slot3")
        setSlot(3, ref);
    else if (name == "slot4")
        setSlot(4, ref);
    else if (name == "rom")
        rom = ref;
    else if (name == "memoryFF00")
        memoryFF00 = ref;
    else if (name == "systemControl")
    {
        if (systemControl)
        {
            systemControl->removeObserver(this, APPLEIII_ENVIRONMENT_DID_CHANGE);
            systemControl->removeObserver(this, APPLEIII_APPLEIIMODE_DID_CHANGE);
        }
        systemControl = ref;
        if (systemControl)
        {
            systemControl->addObserver(this, APPLEIII_ENVIRONMENT_DID_CHANGE);
            systemControl->addObserver(this, APPLEIII_APPLEIIMODE_DID_CHANGE);
        }
    }
    else
        return AddressDecoder::setRef(name, ref);
    
    return true;
}

bool AppleIIIAddressDecoder::init()
{
    OECheckComponent(bankSwitcher);
    OECheckComponent(io);
    OECheckComponent(rom);
    OECheckComponent(memoryFF00);
    OECheckComponent(systemControl);
    
    MemoryMap m;
    
    m.read = true;
    m.write = true;
    
    // C000-C0FF
    m.startAddress = 0xc000;
    m.endAddress = 0xc0ff;
    m.component = io;
    ioMemoryMaps.push_back(m);
    
    // C500-C7FF
    m.startAddress = 0xc500;
    m.endAddress = 0xc7ff;
    m.component = bankSwitcher;
    ioMemoryMaps.push_back(m);
    ioC500Map = &ioMemoryMaps.back();
    
    // C000-CFFF
    m.startAddress = 0xc000;
    m.endAddress = 0xcfff;
    m.component = bankSwitcher;
    ramMemoryMaps.push_back(m);
    ramC000Map = &ramMemoryMaps.back();
    
    // D000-EFFF
    m.startAddress = 0xd000;
    m.endAddress = 0xefff;
    m.component = bankSwitcher;
    appleIIIMemoryMaps.push_back(m);
    ramD000Map = &appleIIIMemoryMaps.back();
    
    // F000-FEFF
    m.startAddress = 0xf000;
    m.endAddress = 0xfeff;
    m.read = true;
    m.write = false;
    m.component = bankSwitcher;
    appleIIIMemoryMaps.push_back(m);
    ramF000RMap = &appleIIIMemoryMaps.back();
    
    m.startAddress = 0xf000;
    m.endAddress = 0xfeff;
    m.read = false;
    m.write = true;
    m.component = bankSwitcher;
    appleIIIMemoryMaps.push_back(m);
    ramF000WMap = &appleIIIMemoryMaps.back();
    
    m.read = true;
    m.write = true;
    
    // FF00-FFFF
    m.startAddress = 0xff00;
    m.endAddress = 0xffff;
    m.component = memoryFF00;
    appleIIIMemoryMaps.push_back(m);
    ramFF00Map = &appleIIIMemoryMaps.back();
    
    // FF00-FFBF memory
    ff00RMemoryMap.startAddress = 0x00;
    ff00RMemoryMap.endAddress = 0xbf;
    ff00RMemoryMap.read = true;
    ff00RMemoryMap.write = false;
    ff00RMemoryMap.component = NULL;
    
    ff00WMemoryMap.startAddress = 0x00;
    ff00WMemoryMap.endAddress = 0xbf;
    ff00WMemoryMap.read = false;
    ff00WMemoryMap.write = true;
    ff00WMemoryMap.component = bankSwitcher;
    
    // FFC0-FFCF memory
    ffc0MemoryMap.startAddress = 0xc0;
    ffc0MemoryMap.endAddress = 0xcf;
    ffc0MemoryMap.read = true;
    ffc0MemoryMap.write = true;
    ffc0MemoryMap.component = bankSwitcher;
    
    // FFF0-FFFF memory
    fff0RMemoryMap.startAddress = 0xf0;
    fff0RMemoryMap.endAddress = 0xff;
    fff0RMemoryMap.read = true;
    fff0RMemoryMap.write = false;
    fff0RMemoryMap.component = NULL;
    
    fff0WMemoryMap.startAddress = 0xf0;
    fff0WMemoryMap.endAddress = 0xff;
    fff0WMemoryMap.read = false;
    fff0WMemoryMap.write = true;
    fff0WMemoryMap.component = bankSwitcher;
    
    systemControl->postMessage(this, APPLEIII_GET_ENVIRONMENT, &environment);
    systemControl->postMessage(this, APPLEIII_GET_APPLEIIMODE, &appleIIMode);
    
    updateAppleIIIMemoryMaps();
    
    if (!AddressDecoder::init())
        return false;
    
    return true;
}

bool AppleIIIAddressDecoder::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case ADDRESSDECODER_MAP:
            return addMemoryMap(externalMemoryMaps, (MemoryMap *) data);
            
        case ADDRESSDECODER_UNMAP:
            return removeMemoryMap(externalMemoryMaps, (MemoryMap *) data);
            
        case APPLEII_MAP_SLOT:
            return addMemoryMap(ioMemoryMaps, (MemoryMap *) data);
            
        case APPLEII_UNMAP_SLOT:
            return removeMemoryMap(ioMemoryMaps, (MemoryMap *) data);
    }
    
    return false;
}

void AppleIIIAddressDecoder::notify(OEComponent *sender, int notification, void *data)
{
    switch (notification)
    {
        case APPLEIII_ENVIRONMENT_DID_CHANGE:
            setEnvironment(*((OEChar *)data));
            
            return;
            
        case APPLEIII_APPLEIIMODE_DID_CHANGE:
            setAppleIIMode(*((bool *)data));
            
            return;
    }
}

void AppleIIIAddressDecoder::updateReadWriteMap(OEAddress startAddress, OEAddress endAddress)
{
    AddressDecoder::updateReadWriteMap(internalMemoryMaps, startAddress, endAddress);
    AddressDecoder::updateReadWriteMap(appleIIIMemoryMaps, startAddress, endAddress);
    AddressDecoder::updateReadWriteMap(OEGetBit(environment, APPLEIII_IOENABLE) ?
                                       ioMemoryMaps : ramMemoryMaps,
                                       startAddress, endAddress);
    AddressDecoder::updateReadWriteMap(externalMemoryMaps, startAddress, endAddress);
}

void AppleIIIAddressDecoder::setSlot(OEInt index, OEComponent *ref)
{
    MemoryMap m;
    
    m.startAddress = 0xc000 + 0x100 * index;
    m.endAddress = 0xc0ff + 0x100 * index;
    m.read = true;
    m.write = true;
    m.component = ref;
    
    if (ref)
        addMemoryMap(ioMemoryMaps, &m);
    else
        removeMemoryMap(ioMemoryMaps, &m);
    
    if (readMapp)
        updateReadWriteMap(m.startAddress, m.endAddress);
}

bool AppleIIIAddressDecoder::setEnvironment(OEChar value)
{
    if (environment != value)
    {
        environment = value;
        
        updateAppleIIIMemoryMaps();
        
        updateReadWriteMap(0xc000, 0xffff);
    }
    
    return true;
}

bool AppleIIIAddressDecoder::setAppleIIMode(bool value)
{
    if (appleIIMode != value)
    {
        appleIIMode = value;
        
        updateAppleIIIMemoryMaps();
        
        updateReadWriteMap(0xff00, 0xffff);
    }
    
    return true;
}

void AppleIIIAddressDecoder::updateAppleIIIMemoryMaps()
{
    bool ramWP = OEGetBit(environment, APPLEIII_RAMWP);
    bool romEnabled = OEGetBit(environment, APPLEIII_ROMSEL1);
    
    ioC500Map->write = !ramWP;
    ramC000Map->write = !ramWP;
    ramD000Map->write = !ramWP;
    ramF000RMap->component = romEnabled ? rom : bankSwitcher;
    ramF000WMap->write = !ramWP;
    ramFF00Map->component = appleIIMode ? (romEnabled ? rom : bankSwitcher) : memoryFF00;
    ramFF00Map->write = appleIIMode ? !ramWP : true;
    
    // Map FF00 memory
    memoryFF00->postMessage(this, ADDRESSDECODER_UNMAP, &ff00RMemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_UNMAP, &ff00WMemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_UNMAP, &ffc0MemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_UNMAP, &fff0RMemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_UNMAP, &fff0WMemoryMap);
    
    ff00RMemoryMap.component = fff0RMemoryMap.component = romEnabled ? rom : bankSwitcher;
    ff00WMemoryMap.write = ffc0MemoryMap.write = fff0WMemoryMap.write = !ramWP;
    
    memoryFF00->postMessage(this, ADDRESSDECODER_MAP, &ff00RMemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_MAP, &ff00WMemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_MAP, &ffc0MemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_MAP, &fff0RMemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_MAP, &fff0WMemoryMap);
}
