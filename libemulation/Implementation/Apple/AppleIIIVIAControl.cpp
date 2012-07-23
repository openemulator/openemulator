
/**
 * libemulation
 * Apple III VIA Control
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III VIA control
 */

#include "AppleIIIVIAControl.h"

#include "MemoryInterface.h"

AppleIIIVIAControl::AppleIIIVIAControl()
{
    ramBanking = NULL;
    dacAudioCodec = NULL;
    
    environment = 0;
    zeroPage = 0;
}

bool AppleIIIVIAControl::setRef(string name, OEComponent *ref)
{
    if (name == "ramBanking")
        ramBanking = ref;
    else if (name == "dacAudioCodec")
        dacAudioCodec = ref;
    else
        return false;
    
    return true;
}

bool AppleIIIVIAControl::init()
{
    if (!ramBanking)
    {
        logMessage("ramBanking not connected");
        
        return false;
    }
    
    if (!dacAudioCodec)
    {
        logMessage("dacAudioCodec not connected");
        
        return false;
    }
    
    return true;
}

OEChar AppleIIIVIAControl::read(OEAddress address)
{
    logMessage("R " + getString(address));
    
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

void AppleIIIVIAControl::write(OEAddress address, OEChar value)
{
    switch (address)
    {
        case 0x0:
            environment = value;
            
            break;
            
        case 0x1:
        {
            zeroPage = value;
            
            AddressOffsetMap offsetMap;
            
            offsetMap.startAddress = 0x0000;
            offsetMap.endAddress = 0x00ff;
            offsetMap.offset = 0x100 * zeroPage;
            
            ramBanking->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
            
            if (OEGetBit(environment, (1 << 1)))
            {
                offsetMap.startAddress = 0x0100;
                offsetMap.endAddress = 0x01ff;
                offsetMap.offset = 0x100 * (zeroPage ^ 0x01);
                
                ramBanking->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
            }
                
            break;
        }            
        case 0x2:
        {
            OEChar bank = value & 0xf;
            
            AddressOffsetMap offsetMap;
            
            offsetMap.startAddress = 0x2000;
            offsetMap.endAddress = 0x9fff;
            offsetMap.offset = 0x8000 * bank + 0x6000;
            
            ramBanking->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
            
//            bool appleIISwitch = OEGetBit(value, (1 << 6));
            
            break;
        }
        case 0x3:
        {
            OEChar audioSample = value << 2;
//            bool BL = OEGetBit(value, (1 << 6));
//            bool ioNMI = OEGetBit(value, (1 << 7));
            
            dacAudioCodec->write(0, audioSample);
            dacAudioCodec->write(1, audioSample);
        }
    }
    
    logMessage("W " + getString(address) + ":" + getHexString(value));
}
