
/**
 * libemulation
 * R&D CFFA1
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
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

#define ATA_ERR             (1 << 0)
#define ATA_DRQ             (1 << 3)
#define ATA_DSC             (1 << 4)
#define ATA_RDY             (1 << 6)
#define ATA_BSY             (1 << 7)

#define ATA_IDENTIFY_SIZE   114

RDCFFA1::RDCFFA1()
{
    forceWriteProtected = false;
    
    device = NULL;
    ram = NULL;
    rom = NULL;
    memoryBus = NULL;
    
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
    if (name == "diskImage")
        openDiskImage(value);
    else if (name == "forceWriteProtected")
        forceWriteProtected = getOEInt(value);
    else
        return false;
    
    return true;
}

bool RDCFFA1::getValue(string name, string& value)
{
    if (name == "diskImage")
        value = diskImagePath;
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
    else if (name == "memoryBus")
        memoryBus = ref;
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
    
    if (!memoryBus)
    {
        logMessage("memoryBus not connected");
        
        return false;
    }
    
    mapMemory(ADDRESSDECODER_MAP_MEMORYMAPS);
    
    return true;
}

void RDCFFA1::dispose()
{
    mapMemory(ADDRESSDECODER_UNMAP_MEMORYMAPS);
}

bool RDCFFA1::postMessage(OEComponent *sender, int message, void *data)
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

OEChar RDCFFA1::read(OEAddress address)
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
        {
            // ATAStatus
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

void RDCFFA1::write(OEAddress address, OEChar value)
{
    if ((address & 0xe0) != 0xe0)
        return;
    
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

void RDCFFA1::mapMemory(int message)
{
    MemoryMaps m;
    MemoryMap theMap;
    
    theMap.component = ram;
    theMap.startAddress = 0x1000;
    theMap.endAddress = 0x8fff;
    theMap.read = true;
    theMap.write = true;
    m.push_back(theMap);
    
    theMap.component = rom;
    theMap.startAddress = 0x9000;
    theMap.endAddress = 0xaeff;
    theMap.read = true;
    theMap.write = true;
    m.push_back(theMap);
    
    theMap.component = this;
    theMap.startAddress = 0xaf00;
    theMap.endAddress = 0xafff;
    theMap.read = true;
    theMap.write = true;
    m.push_back(theMap);
    
    if (memoryBus)
        memoryBus->postMessage(this, message, &m);
}

bool RDCFFA1::openDiskImage(string path)
{
    closeDiskImage();
    
    if (!blockStorage.open(path))
        return false;
    
    diskImagePath = path;
    
    return true;
}

void RDCFFA1::closeDiskImage()
{
    blockStorage.close();
    
    diskImagePath = "";
}
