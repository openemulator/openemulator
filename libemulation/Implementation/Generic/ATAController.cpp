
/**
 * libemulation
 * ATA Controller
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a generic ATA Controller
 */

#include <string.h>

#include "ATAController.h"

#include "DeviceInterface.h"
#include "StorageInterface.h"
#include "MemoryInterface.h"

#define ATA_READ                0x20
#define ATA_WRITE               0x30
#define ATA_IDENTIFY            0xec
#define ATA_SET_FEATURE         0xef

#define ATA_DRIVE               (1 << 4)

#define ATA_ERR                 (1 << 0)
#define ATA_DRQ                 (1 << 3)
#define ATA_DSC                 (1 << 4)
#define ATA_RDY                 (1 << 6)
#define ATA_BSY                 (1 << 7)

#define ATA_SRST                (1 << 2)

#define ATA_SERIAL              (10 * 2)
#define ATA_SERIAL_SIZE         (10 * 2)
#define ATA_FIRMWARE_VER        (23 * 2)
#define ATA_FIRMWARE_VER_SIZE   (4 * 2)
#define ATA_MODEL               (27 * 2)
#define ATA_MODEL_SIZE          (20 * 2)
#define ATA_SIZE                (60 * 2)

ATAController::ATAController()
{
    forceWriteProtected = false;
    
    device = NULL;
    
    drive = 0;
    command = 0;
    feature = 0;
    status = ATA_RDY | ATA_DSC;
    lba.q = 0;
    bufferIndex = 0;
    
    byteMode = false;
}

ATAController::~ATAController()
{
    closeDiskImage();
}

bool ATAController::setValue(string name, string value)
{
    if (name == "diskImage")
        openDiskImage(value);
    else if (name == "forceWriteProtected")
        forceWriteProtected = getOEInt(value);
    else
        return false;
    
    return true;
}

bool ATAController::getValue(string name, string& value)
{
    if (name == "diskImage")
        value = diskImagePath;
    else if (name == "forceWriteProtected")
        value = getString(forceWriteProtected);
    else
        return false;
    
    return true;
}

bool ATAController::setRef(string name, OEComponent *ref)
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

bool ATAController::init()
{
    if (!device)
    {
        logMessage("device not connected");
        
        return false;
    }
    
    return true;
}

bool ATAController::postMessage(OEComponent *sender, int message, void *data)
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
            *((string *)data) = blockStorage[drive].getFormatLabel();
            
            return true;
    }
    
    return false;
}

OEChar ATAController::read(OEAddress address)
{
    logMessage("r " + getHexString(address));
    
    switch (address & 0x7)
    {
        case 0:
        {
            // ATA Data
            OEChar value = buffer[bufferIndex++];
            
            if (bufferIndex == ATA_BUFFER_SIZE)
            {
                OEClearBit(status, ATA_DRQ);
                
                bufferIndex = 0;
                lba.d.l++;
            }
            
            return value;
        }
            
        case 1:
            // ATA Error
            return 0;
            
        case 2:
            // ATA Sector count
            return sectorCount;
            
        case 3:
            // ATA LBA 00-07
            return lba.b.l;
            
        case 4:
            // ATA LBA 08-15
            return lba.b.h;
            
        case 5:
            // ATA LBA 16-23
            return lba.b.h2;
            
        case 6:
            // ATA LBA 24-27
            return lba.b.h3 | (drive ? ATA_DRIVE : 0);
            
        case 7:
            // ATA Status
            return status;
    }
    
    return 0;
}

void ATAController::write(OEAddress address, OEChar value)
{
    logMessage("w " + getHexString(address) + ": " + getHexString(value));
    
    switch (address & 0x7)
    {
        case 0:
            // ATA Data
            if (command != ATA_WRITE)
                OEAssertBit(status, ATA_ERR);
            else
            {
                buffer[bufferIndex++] = value;
                
                if (bufferIndex == ATA_BUFFER_SIZE)
                {
                    OEClearBit(status, ATA_DRQ);
                    
                    if (blockStorage[drive].isWriteEnabled() && !forceWriteProtected)
                    {
                        if (!blockStorage[drive].writeBlocks(lba.d.l, buffer, 1))
                            OEAssertBit(status, ATA_ERR);
                    }
                    else
                        OEAssertBit(status, ATA_ERR);
                    
                    bufferIndex = 0;
                    lba.d.l++;
                }
            }
            
            break;
            
        case 1:
            // ATA feature
            feature = value;
            
            break;
            
        case 2:
            // ATA sector count
            sectorCount = value;
            
            break;
            
        case 3:
            // ATA LBA 00-07
            lba.b.l = value;
            
            break;
            
        case 4:
            // ATA LBA 08-15
            lba.b.h = value;
            
            break;
            
        case 5:
            // ATA LBA 16-23
            lba.b.h2 = value;
            
            break;
            
        case 6:
            // ATA LBA 24-27
            lba.b.h3 = value & 0xf;
            drive = OEGetBit(value, ATA_DRIVE);
            
            break;
            
        case 7:
        {
            // ATA Command
            command = value;
            
            OEClearBit(status, ATA_ERR);
            OEClearBit(status, ATA_DRQ);
            
            switch (command)
            {
                case ATA_READ:
                    bufferIndex = 0;
                    
                    if (!blockStorage[drive].readBlocks(lba.d.l, buffer, 1))
                        OEAssertBit(status, ATA_ERR);
                    else
                        OEAssertBit(status, ATA_DRQ);
                    
                    break;
                    
                case ATA_WRITE:
                    bufferIndex = 0;
                    
                    if (!blockStorage[drive].isWriteEnabled() || forceWriteProtected)
                        OEAssertBit(status, ATA_ERR);
                    else
                        OEAssertBit(status, ATA_DRQ);
                    
                    break;
                    
                case ATA_IDENTIFY:
                {
                    // Identify
                    bufferIndex = 0;
                    
                    if (blockStorage[drive].isOpen())
                    {
                        OEUnion lbaSize;
                        lbaSize.q = blockStorage[drive].getBlockNum();
                        
                        memset(buffer, 0, ATA_BUFFER_SIZE);
                        
                        strncpy((char *) &buffer[ATA_MODEL],
                                diskImagePath.c_str(),
                                ATA_MODEL_SIZE);
                        
                        buffer[ATA_SIZE + 0] = lbaSize.b.l;
                        buffer[ATA_SIZE + 1] = lbaSize.b.h;
                        buffer[ATA_SIZE + 2] = lbaSize.b.h2;
                        buffer[ATA_SIZE + 3] = lbaSize.b.h3;
                        
                        OEAssertBit(status, ATA_DRQ);
                    }
                    else
                        OEAssertBit(status, ATA_ERR);
                    
                    break;
                }
                case ATA_SET_FEATURE:
                {
                    switch (feature)
                    {
                        case 0x01:
                            // Enable 8-bit PIO transfer mode
                            byteMode = true;
                            
                            break;
                            
                        case 0x81:
                            // Disable 8-bit PIO transfer mode
                            byteMode = false;
                            
                            break;
                            
                        default:
                            OEAssertBit(status, ATA_ERR);
                            
                            break;
                    }
                }
                default:
                    OEAssertBit(status, ATA_ERR);
                    
                    break;
            }
            
            break;
        }
    }
}

bool ATAController::openDiskImage(string path)
{
    closeDiskImage();
    
    if (!blockStorage[0].open(path))
        return false;
    
    diskImagePath = path;
    
    return true;
}

void ATAController::closeDiskImage()
{
    blockStorage[0].close();
    
    diskImagePath = "";
}
