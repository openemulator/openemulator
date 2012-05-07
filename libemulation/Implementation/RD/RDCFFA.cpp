
/**
 * libemulation
 * R&D CFFA
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an R&D CFFA interface card
 */

#include "RDCFFA.h"

#include "DeviceInterface.h"
#include "StorageInterface.h"
#include "MemoryInterface.h"

#define ATA_READ            0x20
#define ATA_WRITE           0x30
#define ATA_IDENTIFY        0xec
#define ATA_SET_FEATURE     0xef

#define ATA_ERR             (1 << 0)
#define ATA_DRQ             (1 << 3)
#define ATA_DSC             (1 << 4)
#define ATA_RDY             (1 << 6)
#define ATA_BSY             (1 << 7)

#define ATA_IDENTIFY_SIZE   114

RDCFFA::RDCFFA()
{
    forceWriteProtected = false;
    
    device = NULL;
    
    ataBufferIndex = 0;
    ataError = false;
    ataLBA.q = 0;
    ataCommand = 0;
}

RDCFFA::~RDCFFA()
{
    closeDiskImage();
}

bool RDCFFA::setValue(string name, string value)
{
    if (name == "diskImage")
        openDiskImage(value);
    else if (name == "forceWriteProtected")
        forceWriteProtected = getOEInt(value);
    else
        return false;
    
    return true;
}

bool RDCFFA::getValue(string name, string& value)
{
    if (name == "diskImage")
        value = diskImagePath;
    else if (name == "forceWriteProtected")
        value = getString(forceWriteProtected);
    else
        return false;
    
    return true;
}

bool RDCFFA::setRef(string name, OEComponent *ref)
{
    if (name == "device")
    {
        if (device)
            device->postMessage(this, DEVICE_REMOVE_STORAGE, this);
        device = ref;
        if (device)
            device->postMessage(this, DEVICE_ADD_STORAGE, this);
    }
    else
        return false;
    
    return true;
}

bool RDCFFA::init()
{
    if (!device)
    {
        logMessage("device not connected");
        
        return false;
    }
    
    return true;
}

bool RDCFFA::postMessage(OEComponent *sender, int message, void *data)
{
    switch(message)
    {
        case STORAGE_IS_AVAILABLE:
            return true;
            
        case STORAGE_CAN_MOUNT:
        {
            DIAppleBlockStorage blockStorage;
            
            return blockStorage.open(*((string *)data));
        }
            
        case STORAGE_MOUNT:
            if (openDiskImage(*((string *)data)))
            {
                device->postMessage(this, DEVICE_UPDATE, NULL);
                
                return true;
            }
            
            return false;
            
        case STORAGE_UNMOUNT:
            closeDiskImage();
            
            device->postMessage(this, DEVICE_UPDATE, NULL);
            
            return true;
            
        case STORAGE_GET_MOUNTPATH:
            *(string *)data = diskImagePath;
            
            return true;
            
        case STORAGE_IS_LOCKED:
            return false;
            
        case STORAGE_GET_FORMATLABEL:
            *((string *)data) = blockStorage.getFormatLabel();
            
            return true;
    }
    
    return false;
}

OEChar RDCFFA::read(OEAddress address)
{
    switch (address & 0xf)
    {
        case 0x08:
            // ATA Data
            if (ataBufferIndex >= 0x200)
                return 0;
            
            return ataBuffer[ataBufferIndex++];
            
            
        case 0x09:
            // ATA Error
            return 0;
            
        case 0x0a:
            // ATA Sector count
            return 0;
            
        case 0x0b:
            // ATA LBA 00-07
            return ataLBA.b.l;
            
        case 0x0c:
            // ATA LBA 08-15
            return ataLBA.b.h;
            
        case 0x0d:
            // ATA LBA 16-23
            return ataLBA.b.h2;
            
        case 0x0e:
            // ATA LBA 24-27
            return ataLBA.b.h3;
            
        case 0x0f:
        {
            // ATA Status
            OEChar status = 0;
            
            if (ataError)
                OEAssertBit(status, ATA_ERR);
            else
                OESetBit(status, ATA_DRQ, ataBufferIndex < 0x200);
            OEAssertBit(status, ATA_DSC);
            OEAssertBit(status, ATA_RDY);
            
            return status;
        }
            
        default:
            return 0;
    }
}

void RDCFFA::write(OEAddress address, OEChar value)
{
    switch (address & 0xf)
    {
        case 0x08:
            // ATA Data
            if (ataBufferIndex >= 0x200)
                break;
            
            ataBuffer[ataBufferIndex++] = value;
            
            if ((ataBufferIndex == 0x200) &&
                (ataCommand == ATA_WRITE))
            {
                if (blockStorage.isWriteEnabled() && !forceWriteProtected)
                    ataError = !blockStorage.writeBlocks(ataLBA.d.l, ataBuffer, 1);
                else
                    ataError = true;
            }
            
            break;
            
        case 0x0b:
            // ATA LBA 00-07
            ataLBA.b.l = value;
            
            break;
            
        case 0x0c:
            // ATA LBA 08-15
            ataLBA.b.h = value;
            
            break;
            
        case 0x0d:
            // ATA LBA 16-23
            ataLBA.b.h2 = value;
            
            break;
            
        case 0x0e:
            // ATA LBA 24-27
            ataLBA.b.h3 = value & 0xf;
            
            break;
            
        case 0x0f:
        {
            // ATA Command
            ataCommand = value;
            
            switch (ataCommand)
            {
                case ATA_READ:
                    ataBufferIndex = 0;
                    
                    ataError = !blockStorage.readBlocks(ataLBA.d.l, ataBuffer, 1);
                    
                    break;
                    
                case ATA_WRITE:
                    ataBufferIndex = 0;
                    
                    ataError = !blockStorage.isWriteEnabled() || forceWriteProtected;
                    
                    break;
                    
                case ATA_IDENTIFY:
                {
                    // Identify
                    ataBufferIndex = 0;
                    
                    if (blockStorage.isOpen())
                    {
                        OEUnion lbaSize;
                        lbaSize.q = blockStorage.getBlockNum();
                        
                        memset(ataBuffer, 0, 0x200);
                        
                        ataBuffer[ATA_IDENTIFY_SIZE + 0] = lbaSize.b.l;
                        ataBuffer[ATA_IDENTIFY_SIZE + 1] = lbaSize.b.h;
                        ataBuffer[ATA_IDENTIFY_SIZE + 2] = lbaSize.b.h2;
                        ataBuffer[ATA_IDENTIFY_SIZE + 3] = lbaSize.b.h3;
                        
                        ataError = false;
                    }
                    else
                        ataError = true;
                    
                    break;
                }
                    
                case ATA_SET_FEATURE:
                    ataError = false;
                    
                    break;
                    
                default:
                    ataError = true;
                    
                    break;
            }
            
            break;
        }
    }
}

bool RDCFFA::openDiskImage(string path)
{
    closeDiskImage();
    
    if (!blockStorage.open(path))
        return false;
    
    diskImagePath = path;
    
    return true;
}

void RDCFFA::closeDiskImage()
{
    blockStorage.close();
    
    diskImagePath = "";
}
