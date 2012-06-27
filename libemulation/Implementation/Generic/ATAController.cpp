
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

// ATA commands
#define ATA_READ                0x20
#define ATA_WRITE               0x30
#define ATA_IDENTIFY            0xec
#define ATA_SET_FEATURE         0xef

// LBA24-27 codes
#define ATA_DRIVE               0x10
#define ATA_ADDRESSMODEMASK     0xe0
#define ATA_CHS                 0xa0
#define ATA_LBA28               0xe0
#define ATA_LBA48               0x40

// ATA status codes
#define ATA_ERR                 (1 << 0)
#define ATA_DRQ                 (1 << 3)
#define ATA_DSC                 (1 << 4)
#define ATA_DF                  (1 << 5)
#define ATA_RDY                 (1 << 6)
#define ATA_BSY                 (1 << 7)

// ATA device control bits
#define ATA_SRST                (1 << 2)

// ATA identify fields
#define ATA_SERIAL              (10 * 2)
#define ATA_SERIAL_SIZE         (10 * 2)
#define ATA_FIRMWARE_VER        (23 * 2)
#define ATA_FIRMWARE_VER_SIZE   (4 * 2)
#define ATA_MODEL               (27 * 2)
#define ATA_MODEL_SIZE          (20 * 2)
#define ATA_SIZE                (57 * 2)
#define ATA_SIZE2               (60 * 2)

ATAController::ATAController()
{
    drive[0] = NULL;
    drive[1] = NULL;
    
    feature = 0;
    status = ATA_RDY | ATA_DSC;
    command = 0;
    
    lba.q = 0;
    sectorCount = 0;
    
    bufferIndex = 0;
    
    driveSel = 0;
    addressMode = ATA_CHS;
    pioByteMode = false;
}

bool ATAController::setRef(string name, OEComponent *ref)
{
    if (name == "drive1")
        drive[0] = ref;
    else if (name == "drive2")
        drive[1] = ref;
    else
        return false;
    
    return true;
}

bool ATAController::init()
{
    selectDrive(0);
    
    return true;
}

void ATAController::update()
{
    selectDrive(driveSel);
}

OEChar ATAController::read(OEAddress address)
{
    return read16(address);
}

void ATAController::write(OEAddress address, OEChar value)
{
    write16(address, value);
}

OEShort ATAController::read16(OEAddress address)
{
    if (address & 0x200)
        // ATA alt status
        return status;
    else
    {
        switch (address & 0x7)
        {
            case 0:
            {
                // ATA Data
                OEShort value = buffer[bufferIndex++];
                if (!pioByteMode)
                    value |= (buffer[bufferIndex++] << 8);
                
                if (bufferIndex == ATA_BUFFER_SIZE)
                {
                    OEClearBit(status, ATA_DRQ);
                    
                    bufferIndex = 0;
                    lba.d.l++;
                }
                
                return value;
            }
            case 1:
                // ATA error
                return 0;
                
            case 2:
                // ATA sector count
                return sectorCount;
                
            case 3:
                // ATA sector number
                return lba.b.l;
                
            case 4:
                // ATA cylinder low
                return lba.b.h;
                
            case 5:
                // ATA cylinder high
                return lba.b.h2;
                
            case 6:
                // ATA drive/head
                return lba.b.h3 | (driveSel ? ATA_DRIVE : 0) | addressMode;
                
            case 7:
                // ATA status
                return status;
        }
    }
    
    return 0;
}

void ATAController::write16(OEAddress address, OEShort value)
{
    if (address & 0x200)
    {
        // ATA device control
        if (OEGetBit(value, ATA_SRST))
        {
            // Software reset
            OEClearBit(status, ATA_ERR);
            OEClearBit(status, ATA_DRQ);
            
            command = 0;
            
            bufferIndex = 0;
        }
    }
    else
    {
        switch (address & 0x7)
        {
            case 0:
                // ATA data
                if (command == ATA_WRITE)
                {
                    buffer[bufferIndex++] = value;
                    if (!pioByteMode)
                        buffer[bufferIndex++] = (value >> 8);
                    
                    if (bufferIndex == ATA_BUFFER_SIZE)
                    {
                        if (!blockStorage->writeBlocks(lba.d.l, buffer, 1))
                            OEAssertBit(status, ATA_ERR);
                        
                        OEClearBit(status, ATA_DRQ);
                        
                        bufferIndex = 0;
                        lba.d.l++;
                    }
                }
                else
                    OEAssertBit(status, ATA_ERR);
                
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
                // ATA sector number
                lba.b.l = value;
                
                break;
                
            case 4:
                // ATA cylinder low
                lba.b.h = value;
                
                break;
                
            case 5:
                // ATA cylinder high
                lba.b.h2 = value;
                
                break;
                
            case 6:
                // ATA drive/head
                lba.b.h3 = value & 0xf;
                
                selectDrive(OEGetBit(value, ATA_DRIVE));
                
                addressMode = value & ATA_ADDRESSMODEMASK;
                if ((addressMode != ATA_CHS) &&
                    (addressMode != ATA_LBA28))
                    OEAssertBit(status, ATA_ERR);
                
                break;
                
            case 7:
                // ATA command
                command = value;
                
                OEClearBit(status, ATA_ERR);
                OEClearBit(status, ATA_DRQ);
                
                switch (command)
                {
                    case ATA_READ:
                        bufferIndex = 0;
                        
                        if (blockStorage->readBlocks(lba.d.l, buffer, 1))
                            OEAssertBit(status, ATA_DRQ);
                        else
                            OEAssertBit(status, ATA_ERR);
                        
                        break;
                        
                    case ATA_WRITE:
                        bufferIndex = 0;
                        
                        if (blockStorage->isWriteEnabled())
                            OEAssertBit(status, ATA_DRQ);
                        else
                            OEAssertBit(status, ATA_ERR);
                        
                        break;
                        
                    case ATA_IDENTIFY:
                    {
                        // Identify
                        bufferIndex = 0;
                        
                        if (blockStorage->isOpen())
                        {
                            OEUnion lbaSize;
                            lbaSize.q = blockStorage->getBlockNum();
                            
                            memset(buffer, 0, ATA_BUFFER_SIZE);
                            
                            setATAString((char *) buffer + ATA_SERIAL,
                                         blockStorage->getATASerial().c_str(),
                                         ATA_SERIAL_SIZE);
                            setATAString((char *) buffer + ATA_FIRMWARE_VER,
                                         blockStorage->getATAFirmware().c_str(),
                                         ATA_FIRMWARE_VER_SIZE);
                            setATAString((char *) buffer + ATA_MODEL,
                                         blockStorage->getATAModel().c_str(),
                                         ATA_MODEL_SIZE);
                            
                            buffer[ATA_SIZE + 0] = lbaSize.b.l;
                            buffer[ATA_SIZE + 1] = lbaSize.b.h;
                            buffer[ATA_SIZE + 2] = lbaSize.b.h2;
                            buffer[ATA_SIZE + 3] = lbaSize.b.h3;
                            
                            buffer[ATA_SIZE2 + 0] = lbaSize.b.l;
                            buffer[ATA_SIZE2 + 1] = lbaSize.b.h;
                            buffer[ATA_SIZE2 + 2] = lbaSize.b.h2;
                            buffer[ATA_SIZE2 + 3] = lbaSize.b.h3;
                            
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
                                pioByteMode = true;
                                
                                break;
                                
                            case 0x81:
                                // Disable 8-bit PIO transfer mode
                                pioByteMode = false;
                                
                                break;
                                
                            default:
                                logMessage("unknown ATA SET_FEATURE " + getHexString(feature));
                                
                                OEAssertBit(status, ATA_ERR);
                                
                                break;
                        }
                        
                        break;
                    }
                    default:
                        logMessage("unknown ATA COMMAND " + getHexString(command));
                        
                        OEAssertBit(status, ATA_ERR);
                        
                        break;
                }
                
                break;
        }
    }
}

void ATAController::selectDrive(OEInt value)
{
    driveSel = value;
    
    if (!drive[value])
        blockStorage = &dummyBlockStorage;
    else
        drive[value]->postMessage(this, STORAGE_GET_OBJECT, &blockStorage);
}

void ATAController::setATAString(char *dest, const char *src, OEInt size)
{
    for (OEInt i = 0; i < (strlen(src)) && (i < size); i++)
        dest[i ^ 1] = src[i];
}
