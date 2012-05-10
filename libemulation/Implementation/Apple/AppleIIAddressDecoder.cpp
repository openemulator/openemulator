
/**
 * libemulation
 * Apple II Address Decoder
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II Address Decoder
 */

#include "AppleIIAddressDecoder.h"

#include "AppleIIInterface.h"

AppleIIAddressDecoder::AppleIIAddressDecoder() : AddressDecoder()
{
    dummyVRAM.resize(0x2000);
}

bool AppleIIAddressDecoder::setRef(string name, OEComponent *ref)
{
    if (name == "video")
        video = ref;
    else
        return AddressDecoder::setRef(name, ref);
    
    return true;
}

bool AppleIIAddressDecoder::init()
{
    if (!AddressDecoder::init())
        return false;
    
    if (!video)
    {
        logMessage("video not connected");
        
        return false;
    }
    
    videoRefresh.resize((size_t) (size / blockSize));
    videoRefreshp = &videoRefresh.front();
    
    OEInt startBlock, endBlock;
    
    startBlock = 0x400 >> blockBits;
    endBlock = 0x7ff >> blockBits;
    for (OEInt i = startBlock; i <= endBlock; i++)
        videoRefresh[i] = true;
    
    startBlock = 0x2000 >> blockBits;
    endBlock = 0x5fff >> blockBits;
    for (OEInt i = startBlock; i <= endBlock; i++)
        videoRefresh[i] = true;
    
    return true;
}

bool AppleIIAddressDecoder::postMessage(OEComponent *sender, int event, void *data)
{
    switch (event)
    {
        case APPLEII_SET_VRAMMODE:
            // To-Do: Dependiendo del modo, cambiamos el mapa
            
            // Nota: si cambia el mapa, enviamos APPLEII_VRAM_DID_CHANGE
            break;
            
        case APPLEII_GET_VRAM:
        {
            AppleIIVRAM *vram = (AppleIIVRAM *)data;
            
            vram->textMain[0] = getMemory(0x400, 0x7ff);
            vram->textMain[1] = getMemory(0x800, 0xbff);
            vram->hiresMain[0] = getMemory(0x2000, 0x3fff);
            vram->hiresMain[1] = getMemory(0x4000, 0x5fff);
            vram->hbl[0] = getMemory(0x1400, 0x17ff);
            vram->hbl[1] = getMemory(0x1800, 0x1bff);
            vram->textAux[0] = &dummyVRAM.front();
            vram->textAux[1] = &dummyVRAM.front();
            vram->hiresAux[0] = &dummyVRAM.front();
            vram->hiresAux[1] = &dummyVRAM.front();
            
            break;
        }
            
        case APPLEII_MAP_SLOT:
            return addMemoryMap((MemoryMap *) data);
            
        case APPLEII_UNMAP_SLOT:
            return removeMemoryMap((MemoryMap *) data);
            
        case APPLEII_MAP:
            return addMemoryMap((MemoryMap *) data);

        case APPLEII_UNMAP:
            return removeMemoryMap((MemoryMap *) data);
    }
    
    return false;
}

void AppleIIAddressDecoder::write(OEAddress address, OEChar value)
{
    size_t index = (size_t) ((address & mask) >> blockBits);
	writeMapp[index]->write(address, value);
    
    if (videoRefreshp[index])
        video->postMessage(this, APPLEII_REFRESH_VIDEO, NULL);
}

OEChar *AppleIIAddressDecoder::getMemory(OEAddress startAddress, OEAddress endAddress)
{
    for (MemoryMaps::iterator i = staticMemoryMaps.begin();
         i != staticMemoryMaps.end();
         i++)
    {
        if ((i->startAddress <= startAddress) &&
            (i->endAddress >= endAddress))
        {
            OEData *data;
            
            i->component->postMessage(this, RAM_GET_DATA, &data);
            
            return &data->front() + startAddress - i->startAddress;
        }
    }
    
    return &dummyVRAM.front();
}
