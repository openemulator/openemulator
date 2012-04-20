
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

DIAppleDiskImage::DIAppleDiskImage()
{
    close();
}

bool DIAppleDiskImage::open(string path)
{
    return (fileBackingStore.open(path) &&
            open(&fileBackingStore, getDIPathExtension(path)));
}

bool DIAppleDiskImage::open(DIData& data)
{
    return (ramBackingStore.open(data) &&
            open(&ramBackingStore, ""));
}

bool DIAppleDiskImage::open(DIBackingStore *backingStore, string pathExtension)
{
    if (twoImgBackingStore.open(backingStore))
    {
        if (twoImgBackingStore.getFormat() == DI_2IMG_PRODOS)
        {
            if (rawDiskImage.open(&twoImgBackingStore))
            {
                diskImage = &rawDiskImage;
                
                return true;
            }
        }
    }
    else if (dc42BackingStore.open(backingStore))
    {
        if (rawDiskImage.open(&dc42BackingStore))
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
        if ((pathExtension == "image") ||
            (pathExtension == "img") ||
            (pathExtension == "hdf") ||
            (pathExtension == "hdv") ||
            (pathExtension == "po") ||
            (pathExtension == "vdsk"))
        {
            if (rawDiskImage.open(backingStore))
            {
                diskImage = &rawDiskImage;
                
                return true;
            }
        }
    }
    
    return false;
}

bool DIAppleDiskImage::isOpen()
{
    return diskImage != NULL;
}

void DIAppleDiskImage::close()
{
    fileBackingStore.close();
    ramBackingStore.close();
    twoImgBackingStore.close();
    dc42BackingStore.close();
    
    rawDiskImage.close();
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
