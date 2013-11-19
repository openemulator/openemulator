
/**
 * libemulation
 * R&D CFFA ATA Interface
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an R&D CFFA ATA Interface
 */

#include "RDCFFA.h"

RDCFFA::RDCFFA()
{
    ataController = NULL;
    
    csMask = 0;
    ataData = 0;
}

bool RDCFFA::setRef(string name, OEComponent *ref)
{
    if (name == "ataController")
        ataController = ref;
    else
        return false;
    
    return true;
}

bool RDCFFA::init()
{
    OECheckComponent(ataController);
    
    return true;
}

OEChar RDCFFA::read(OEAddress address)
{
    switch (address & 0xf)
    {
        case 0x0:
            return (ataData >> 8);
            
        case 0x1:
            csMask = true;
            
            break;
            
        case 0x2:
            csMask = false;
            
            break;
            
        case 0x6:
            return ataController->read(0x3f6);
            
        case 0x8:
            return csMask ? 0 : (ataData = ataController->read16(0x1f0));
            
        case 0x9:
            return ataController->read(0x1f1);
            
        case 0xa:
            return ataController->read(0x1f2);
            
        case 0xb:
            return ataController->read(0x1f3);
            
        case 0xc:
            return ataController->read(0x1f4);
            
        case 0xd:
            return ataController->read(0x1f5);
            
        case 0xe:
            return ataController->read(0x1f6);
            
        case 0xf:
            return ataController->read(0x1f7);
    }
    
    return 0;
}

void RDCFFA::write(OEAddress address, OEChar value)
{
    switch (address & 0xf)
    {
        case 0x0:
            ataData = value << 8;
            
            break;
            
        case 0x1:
            csMask = true;
            
            break;
            
        case 0x2:
            csMask = false;
            
            break;
            
        case 0x6:
            ataController->write(0x3f6, value);
            
            break;
            
        case 0x8:
            ataController->write16(0x1f0, ataData | value);
            
            break;
            
        case 0x9:
            ataController->write(0x1f1, value);
            
            break;
            
        case 0xa:
            ataController->write(0x1f2, value);
            
            break;
            
        case 0xb:
            ataController->write(0x1f3, value);
            
            break;
            
        case 0xc:
            ataController->write(0x1f4, value);
            
            break;
            
        case 0xd:
            ataController->write(0x1f5, value);
            
            break;
            
        case 0xe:
            ataController->write(0x1f6, value);
            
            break;
            
        case 0xf:
            ataController->write(0x1f7, value);
            
            break;
    }
}
