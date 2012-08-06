
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
    memory = NULL;
    io = NULL;
    
    for (OEInt i = 0; i < 8; i++)
        slot[i] = NULL;
    
    rom = NULL;
    memoryFF00 = NULL;
    
    ioEnabled = true;
}

bool AppleIIIAddressDecoder::setRef(string name, OEComponent *ref)
{
    if (name == "memory")
        memory = ref;
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
    else
        return AddressDecoder::setRef(name, ref);
    
    return true;
}

bool AppleIIIAddressDecoder::init()
{
    if (!memory)
    {
        logMessage("memory not connected");
        
        return false;
    }
    
    if (!io)
    {
        logMessage("io not connected");
        
        return false;
    }
    
    if (!rom)
    {
        logMessage("rom not connected");
        
        return false;
    }
    
    if (!AddressDecoder::init())
        return false;
    
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
    m.component = memory;
    ioMemoryMaps.push_back(m);
    ioC500Map = &ioMemoryMaps.back();
    
    // C000-CFFF
    m.startAddress = 0xc000;
    m.endAddress = 0xcfff;
    m.component = memory;
    ramMemoryMaps.push_back(m);
    ramC000Map = &ramMemoryMaps.back();
    
    // D000-EFFF
    m.startAddress = 0xd000;
    m.endAddress = 0xefff;
    m.component = memory;
    internalMemoryMaps.push_back(m);
    ramD000Map = &internalMemoryMaps.back();
    
    // F000-FEFF
    m.startAddress = 0xf000;
    m.endAddress = 0xfeff;
    m.component = memory;
    internalMemoryMaps.push_back(m);
    ramF000Map = &internalMemoryMaps.back();
    
    // FF00-FFFF
    m.startAddress = 0xff00;
    m.endAddress = 0xffff;
    m.component = memoryFF00;
    internalMemoryMaps.push_back(m);
    ramFF00Map = &internalMemoryMaps.back();
    
    // FF00-FFBF memory
    ff00MemoryMap.startAddress = 0x00;
    ff00MemoryMap.endAddress = 0xbf;
    ff00MemoryMap.read = true;
    ff00MemoryMap.write = true;
    ff00MemoryMap.component = NULL;
    
    // FFC0-FFCF memory
    ffc0MemoryMap.startAddress = 0xc0;
    ffc0MemoryMap.endAddress = 0xcf;
    ffc0MemoryMap.read = true;
    ffc0MemoryMap.write = true;
    ffc0MemoryMap.component = memory;
    
    // FFF0-FFFF memory
    fff0MemoryMap.startAddress = 0xf0;
    fff0MemoryMap.endAddress = 0xff;
    fff0MemoryMap.read = true;
    fff0MemoryMap.write = true;
    fff0MemoryMap.component = NULL;
    
    return true;
}

void AppleIIIAddressDecoder::update()
{
    updateMemoryMaps(0, mask);
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
            
        case APPLEIII_SET_MEMORYCONTROL:
            return setMemoryControl(*((OEInt *)data));
            
        case APPLEIII_SET_APPLEIIMODE:
            return setAppleIIMode(*((bool *)data));
    }
    
    return false;
}

void AppleIIIAddressDecoder::updateMemoryMaps(OEAddress startAddress, OEAddress endAddress)
{
    AddressDecoder::updateMemoryMaps(internalMemoryMaps, startAddress, endAddress);
    AddressDecoder::updateMemoryMaps(ioEnabled ? ioMemoryMaps : ramMemoryMaps,
                                     startAddress, endAddress);
    AddressDecoder::updateMemoryMaps(externalMemoryMaps, startAddress, endAddress);
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
}

bool AppleIIIAddressDecoder::setMemoryControl(OEInt value)
{
    bool ramWP = OEGetBit(value, APPLEIII_RAMWP);
    ioEnabled = OEGetBit(value, APPLEIII_IO);
    bool romEnabled = OEGetBit(value, APPLEIII_ROM);
    
    ioC500Map->write = !ramWP;
    ramC000Map->write = !ramWP;
    ramD000Map->write = !ramWP;
    ramF000Map->write = !ramWP;
    ramF000Map->component = romEnabled ? rom : memory;
    
    updateMemoryMaps(0xc000, 0xffff);
    
    // Map FF00 memory
    memoryFF00->postMessage(this, ADDRESSDECODER_UNMAP, &ff00MemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_UNMAP, &ffc0MemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_UNMAP, &fff0MemoryMap);
    
    ff00MemoryMap.component = fff0MemoryMap.component = romEnabled ? rom : memory;
    ff00MemoryMap.write = ffc0MemoryMap.write = fff0MemoryMap.write = !ramWP;
    
    memoryFF00->postMessage(this, ADDRESSDECODER_MAP, &ff00MemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_MAP, &ffc0MemoryMap);
    memoryFF00->postMessage(this, ADDRESSDECODER_MAP, &fff0MemoryMap);
    
    return true;
}

bool AppleIIIAddressDecoder::setAppleIIMode(bool value)
{
    return true;
}
