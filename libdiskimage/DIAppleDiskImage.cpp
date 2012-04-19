
/**
 * libdiskimage
 * Apple Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple disk image
 */

#include "DIFileBackingStore.h"
#include "DIRAMBackingStore.h"
#include "DIAppleDiskImage.h"

#define SECTOR_SIZE 256

DIAppleDiskImage::DIAppleDiskImage()
{
    close();
}

bool DIAppleDiskImage::open(string path)
{
    close();
    
    if (fileBackingStore.open(path))
    {
        if (open(&fileBackingStore))
            return true;
    }
    
    close();
    
    return false;
}

bool DIAppleDiskImage::open(DIData& data)
{
    close();
    
    if (ramBackingStore.open(data))
    {
        if (open(&ramBackingStore))
            return true;
    }
    
    close();
    
    return false;
}

bool DIAppleDiskImage::open(DIBackingStore *backingStore)
{
    close();
    
    if (twoImgBackingStore.open(backingStore))
    {
        if (rawDiskImage.open(&twoImgBackingStore))
        {
            diskImage = &rawDiskImage;
            
            return true;
        }
        
        return false;
    }
    else if (dc42BackingStore.open(backingStore))
    {
        if (rawDiskImage.open(&twoImgBackingStore))
        {
            diskImage = &rawDiskImage;
            
            return true;
        }
    }
    else if (qcowDiskImage.open(backingStore))
    {
        diskImage = &qcowDiskImage;
        
        return true;
    }
    else if (vmdkDiskImage.open(backingStore))
    {
        diskImage = &vmdkDiskImage;
        
        return true;
    }
    else
    {
        if (rawDiskImage.open(&twoImgBackingStore))
        {
            diskImage = &rawDiskImage;
            
            return true;
        }
    }
    
    return true;
}

void DIAppleDiskImage::close()
{
    fileBackingStore.close();
    ramBackingStore.close();
    twoImgBackingStore.close();
    dc42BackingStore.close();
    qcowDiskImage.close();
    vmdkDiskImage.close();
    
    diskImage = &dummyDiskImage;
}

bool DIAppleDiskImage::isWriteEnabled()
{
    return diskImage->isWriteEnabled();
}

DILong DIAppleDiskImage::getBlockNum()
{
    return diskImage->getBlockNum();
}

string DIAppleDiskImage::getFormatLabel()
{
    return diskImage->getFormatLabel();
}

bool DIAppleDiskImage::readBlocks(DILong index, DIChar *buf, DIInt num)
{
    return diskImage->readBlocks(index, buf, num);
}

bool DIAppleDiskImage::writeBlocks(DILong index, const DIChar *buf, DIInt num)
{
    return diskImage->writeBlocks(index, buf, num);
}
