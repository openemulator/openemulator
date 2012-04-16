
/**
 * libdiskimage
 * Disk Image DiskCopy 4.2
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses DiskCopy 4.2 disk images
 */

#include "DiskImageDC42.h"

#define HEADER_SIZE 0x54

DiskImageDC42::DiskImageDC42()
{
    close();
}

bool DiskImageDC42::open(DiskImageFile *file)
{
    close();
    
    DIChar header[HEADER_SIZE];
    
    if (!file->read(0, header, HEADER_SIZE))
        return false;
    
    // Get image and tag location
    imageOffset = HEADER_SIZE;
    imageSize = getDIIntBE(&header[0x40]);
    
    tagOffset = imageOffset + imageSize;
    tagSize = getDIIntBE(&header[0x44]);
    
    if ((tagOffset + tagSize) != file->getSize())
        return false;
    
    // Get GCR Format byte
    gcrFormat = header[0x51];
    
    // Check id
    if (getDIShortBE(&header[0x52]) != 0x0100)
        return false;
    
    this->file = file;
    
    return true;
}

void DiskImageDC42::close()
{
    file = NULL;
    
    gcrFormat = 0;
    
    imageOffset = 0;
    imageSize = 0;
    
    tagOffset = 0;
    tagSize = 0;
}

DILong DiskImageDC42::getSize()
{
    return imageSize;
}

DIInt DiskImageDC42::getGCRFormat()
{
    return gcrFormat;
}

bool DiskImageDC42::read(DILong pos, DIChar *buf, DILong num)
{
    if ((pos + num) > imageSize)
        return false;
    
    return file->read(imageOffset + pos, buf, num);
}

bool DiskImageDC42::write(DILong pos, const DIChar *buf, DILong num)
{
    if ((pos + num) > imageSize)
        return false;
    
    return file->write(imageOffset + pos, buf, num);
}

bool DiskImageDC42::readTag(DILong pos, DIChar *buf, DILong num)
{
    if ((pos + num) > tagSize)
        return false;
    
    return file->read(tagOffset + pos, buf, num);
}

bool DiskImageDC42::writeTag(DILong pos, const DIChar *buf, DILong num)
{
    if ((pos + num) > tagSize)
        return false;
    
    return file->write(tagOffset + pos, buf, num);
}
