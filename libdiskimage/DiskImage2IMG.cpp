
/**
 * libdiskimage
 * Disk Image 2IMG
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Decodes the 2IMG format
 */

#include "DiskImage2IMG.h"

#define HEADER_SIZE 0x40

DiskImage2IMG::DiskImage2IMG()
{
    close();
}

bool DiskImage2IMG::open(DiskImageFile *file)
{
    close();
    
    DIChar header[HEADER_SIZE];
    
    if (!file->read(0, header, HEADER_SIZE))
        return false;
    
    // Check id
    if (strncmp((char *) &header[0x00], "2IMG", 4))
        return false;
    
    // Check header size
    DIInt headerSize = getDIShortLE(&header[0x08]);
    if (headerSize <= 0x40)
        return false;
    
    // Check 2IMG version
    if (getDIShortLE(&header[0x0a]) != 1)
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
    readOnly = (flags & 0x80000000);
    
    // Get image location
    imageOffset = getDIIntLE(&header[0x18]);
    imageSize = getDIIntLE(&header[0x1c]);
    
    if ((imageOffset + imageSize) > file->getSize())
        return false;
    
    imageOffset = 0;
    imageSize = 0;
    
    gcrVolume = 0;
    
    this->file = file;
    
    return true;
}

void DiskImage2IMG::close()
{
    file = NULL;
    
    readOnly = false;
    sectorOrder = "";
    gcrVolume = 0;
    imageOffset = 0;
    imageSize = 0;
}

bool DiskImage2IMG::isReadOnly()
{
    return readOnly;
}

DILong DiskImage2IMG::getSize()
{
    return imageSize;
}

string DiskImage2IMG::getSectorOrder()
{
    return sectorOrder;
}

DIInt DiskImage2IMG::getGCRVolume()
{
    return gcrVolume;
}

bool DiskImage2IMG::read(DILong pos, DIChar *buf, DILong num)
{
    if ((pos + num) > imageSize)
        return false;
    
    return file->read(imageOffset + pos, buf, num);
}

bool DiskImage2IMG::write(DILong pos, const DIChar *buf, DILong num)
{
    if ((pos + num) > imageSize)
        return false;
    
    return file->write(imageOffset + pos, buf, num);
}
