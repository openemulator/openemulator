
/**
 * libdiskimage
 * 2IMG backing store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a 2IMG backing store
 */

#include "DI2IMGBackingStore.h"

#define HEADER_SIZE 0x40

DI2IMGBackingStore::DI2IMGBackingStore()
{
    close();
}

bool DI2IMGBackingStore::open(DIBackingStore *backingStore)
{
    close();
    
    DIChar header[HEADER_SIZE];
    
    if (!backingStore->read(0, header, HEADER_SIZE))
        return false;
    
    // Check id
    if (strncmp((char *) &header[0x00], "2IMG", 4))
        return false;
    
    // Check header size
    DIInt headerSize = getDIShortLE(&header[0x08]);
    if (headerSize < 0x40)
        return false;
    
    // Check 2IMG version
    if (getDIShortLE(&header[0x0c]) > 1)
        return false;
    
    // Check sector order
    DIInt format = getDIIntLE(&header[0x0c]);
    
    switch (format)
    {
        case 0:
            sectorOrder = "Apple DOS 3.3";
            
            break;
            
        case 1:
            sectorOrder = "Apple ProDOS";
            
            break;
            
        case 2:
            sectorOrder = "Apple NIB";
            
            break;
            
        default:
            return false;
    }
    
    // Get flags
    DIInt flags = getDIIntLE(&header[0x10]);
    
    gcrVolume = (flags & 0x100) ? (flags & 0xff) : 254;
    writeEnabled = !(flags & 0x80000000);
    
    // Get image location
    imageOffset = getDIIntLE(&header[0x18]);
    imageSize = getDIIntLE(&header[0x1c]);
    
    if ((imageOffset + imageSize) > backingStore->getSize())
        return false;
    
    this->backingStore = backingStore;
    
    return true;
}

void DI2IMGBackingStore::close()
{
    backingStore = NULL;
    
    writeEnabled = false;
    sectorOrder = "";
    gcrVolume = 0;
    imageOffset = 0;
    imageSize = 0;
}

bool DI2IMGBackingStore::isWriteEnabled()
{
    return writeEnabled && backingStore->isWriteEnabled();
}

DILong DI2IMGBackingStore::getSize()
{
    return imageSize;
}

string DI2IMGBackingStore::getFormatLabel()
{
    string formatLabel = "2IMG Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

string DI2IMGBackingStore::getSectorOrder()
{
    return sectorOrder;
}

DIInt DI2IMGBackingStore::getGCRVolume()
{
    return gcrVolume;
}

bool DI2IMGBackingStore::read(DILong pos, DIChar *buf, DILong num)
{
    if ((pos + num) > imageSize)
        return false;
    
    return backingStore->read(imageOffset + pos, buf, num);
}

bool DI2IMGBackingStore::write(DILong pos, const DIChar *buf, DILong num)
{
    if ((pos + num) > imageSize)
        return false;
    
    return backingStore->write(imageOffset + pos, buf, num);
}
