
/**
 * libemulation
 * R&D CFFA1
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an R&D CFFA1 interface card
 */

#include "RDCFFA1.h"

#include "DeviceInterface.h"
#include "StorageInterface.h"
#include "MemoryInterface.h"

#define ATA_READ            0x20
#define ATA_WRITE           0x30
#define ATA_IDENTIFY        0xec
#define ATA_SET_FEATURE     0xef

#define ATA_ERR             0x01
#define ATA_DRQ             0x08
#define ATA_DSC             0x10
#define ATA_RDY             0x40
#define ATA_BSY             0x80

#define ATA_IDENTIFY_SIZE   114

RDCFFA1::RDCFFA1()
{
    forceWriteProtected = false;
    
    device = NULL;
    ram = NULL;
    rom = NULL;
    mmu = NULL;
    
    diskImageFP = NULL;
    
    ataBufferIndex = 0;
    ataError = false;
    ataLBA.q = 0;
    ataCommand = 0;
}

RDCFFA1::~RDCFFA1()
{
    closeDiskImage();
}

bool RDCFFA1::setValue(string name, string value)
{
    if (name == "image")
        openDiskImage(value);
    else if (name == "forceWriteProtected")
        forceWriteProtected = getInt(value);
    else
        return false;
    
    return true;
}

bool RDCFFA1::getValue(string name, string& value)
{
    if (name == "image")
        value = diskImage;
    else if (name == "forceWriteProtected")
        value = getString(forceWriteProtected);
    else
        return false;
    
    return true;
}

bool RDCFFA1::setRef(string name, OEComponent *ref)
{
    if (name == "device")
    {
        if (device)
            device->postMessage(this, DEVICE_REMOVE_STORAGE, this);
        device = ref;
        if (device)
            device->postMessage(this, DEVICE_ADD_STORAGE, this);
    }
    else if (name == "ram")
        ram = ref;
    else if (name == "rom")
        rom = ref;
    else if (name == "mmu")
        mmu = ref;
    else
        return false;
    
    return true;
}

bool RDCFFA1::init()
{
    if (!device)
    {
        logMessage("device not connected");
        
        return false;
    }
    
    if (!ram)
    {
        logMessage("ram not connected");
        
        return false;
    }
    
    if (!rom)
    {
        logMessage("rom not connected");
        
        return false;
    }
    
    if (!mmu)
    {
        logMessage("mmu not connected");
        
        return false;
    }
    
    mapMMU(MMU_MAP_MEMORY);
    
    return true;
}

void RDCFFA1::dispose()
{
    mapMMU(MMU_UNMAP_MEMORY);
}

bool RDCFFA1::postMessage(OEComponent *sender, int message, void *data)
{
    switch(message)
    {
        case STORAGE_IS_AVAILABLE:
            return true;
            
        case STORAGE_CAN_MOUNT:
            return true;
            
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
            *(string *)data = diskImage;
            
            return true;
            
        case STORAGE_IS_LOCKED:
            return false;
            
        case STORAGE_GET_FORMATLABEL:
            *((string *)data) = "Read/Write Disk Image";
            
            return true;
    }
    
    return false;
}

OEUInt8 RDCFFA1::read(OEAddress address)
{
    if ((address & 0xe0) != 0xe0)
        return rom->read(address);
    
    switch (address & 0xf)
    {
        case 0x08:
            // ATA Data
            if (ataBufferIndex >= 0x200)
                return 0;
            
            return ataBuffer[ataBufferIndex++];
            
        case 0x0b:
            // ATA_LBA07_00
            return ataLBA.b.l;
            
        case 0x0c:
            // ATA_LBA15_08
            return ataLBA.b.h;
            
        case 0x0d:
            // ATA_LBA23_16 
            return ataLBA.b.h2;
            
        case 0x0e:
            // ATA_LBA27_24
            return ataLBA.b.h3;
            
        case 0x0f:
            // ATAStatus
            return (ATA_RDY |
                    (diskImageFP ? ATA_DSC : 0) |
                    (ataError ? ATA_ERR : ((ataBufferIndex < 0x200) ? ATA_DRQ : 0)));
            
        default:
            return 0;
    }
}

void RDCFFA1::write(OEAddress address, OEUInt8 value)
{
    if ((address & 0xe0) != 0xe0)
        return;
    
    switch (address & 0xf)
    {
        case 0x08:
            // ATA Data
            if (ataBufferIndex >= 0x200)
                return;
            
            ataBuffer[ataBufferIndex++] = value;
            
            if ((ataCommand == ATA_WRITE) &&
                (ataBufferIndex == 0x200) &&
                diskImageFP &&
                !forceWriteProtected)
            {
                fseek(diskImageFP, (size_t) (ataLBA.q << 9), SEEK_SET);
                ataError = !fwrite(ataBuffer, 0x200, 1, diskImageFP);
            }
            
            break;
            
        case 0x0b:
            // ATA_LBA07_00
            ataLBA.b.l = value;
            
            break;
            
        case 0x0c:
            // ATA_LBA15_08
            ataLBA.b.h = value;
            
            break;
            
        case 0x0d:
            // ATA_LBA23_16 
            ataLBA.b.h2 = value;
            
            break;
            
        case 0x0e:
            // ATA_LBA27_24
            ataLBA.b.h3 = value & 0xf;
            
            break;
            
        case 0x0f:
        {
            // ATACommand
            ataCommand = value;
            
            switch (ataCommand)
            {
                case ATA_READ:
                    ataBufferIndex = 0;
                    
                    if (diskImageFP)
                    {
                        fseek(diskImageFP, (size_t) (ataLBA.q << 9), SEEK_SET);
                        ataError = !fread(ataBuffer, 0x200, 1, diskImageFP);
                    }
                    else
                        ataError = true;
                    
                    break;
                    
                case ATA_WRITE:
                    ataBufferIndex = 0;
                    
                    if (diskImageFP &&
                        !forceWriteProtected)
                        ataError = false;
                    else
                        ataError = true;
                    
                    break;
                    
                case ATA_IDENTIFY:
                {
                    // Identify
                    ataBufferIndex = 0;
                    
                    if (diskImageFP)
                    {
                        OEUnion lbaSize;
                        fseek(diskImageFP, 0, SEEK_END);
                        lbaSize.q = ftell(diskImageFP) >> 9;
                        
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

void RDCFFA1::mapMMU(int message)
{
    MemoryMap ramMap, romMap, ioMap;
    
    ramMap.component = ram;
    ramMap.startAddress = 0x1000;
    ramMap.endAddress = 0x8fff;
    ramMap.read = true;
    ramMap.write = true;
    
    romMap.component = rom;
    romMap.startAddress = 0x9000;
    romMap.endAddress = 0xaeff;
    romMap.read = true;
    romMap.write = true;
    
    ioMap.component = this;
    ioMap.startAddress = 0xaf00;
    ioMap.endAddress = 0xafff;
    ioMap.read = true;
    ioMap.write = true;
    
    if (mmu)
    {
        mmu->postMessage(this, message, &ramMap);
        mmu->postMessage(this, message, &romMap);
        mmu->postMessage(this, message, &ioMap);
    }
}

bool RDCFFA1::openDiskImage(string filename)
{
    closeDiskImage();
    
    diskImageFP = fopen(filename.c_str(), "r+");
    if (diskImageFP)
        diskImage = filename;
    
    return diskImageFP != NULL;
}

void RDCFFA1::closeDiskImage()
{
    diskImage = "";
    
    if (diskImageFP)
        fclose(diskImageFP);
    
    diskImageFP = NULL;
}
