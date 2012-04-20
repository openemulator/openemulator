
/**
 * libdiskimage
 * VMDK Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses VMDK disk images
 */

#include "DIVMDKDiskImage.h"

DIVMDKDiskImage::DIVMDKDiskImage()
{
    close();
}

bool DIVMDKDiskImage::open(DIBackingStore *backingStore)
{
    return false;
}

void DIVMDKDiskImage::close()
{
}

bool DIVMDKDiskImage::isWriteEnabled()
{
    return false;
}

DILong DIVMDKDiskImage::getBlockNum()
{
    return 0;
}

string DIVMDKDiskImage::getFormatLabel()
{
    return "";
}

bool DIVMDKDiskImage::readBlocks(DILong index, DIChar *buf, DIInt num)
{
    return false;
}

bool DIVMDKDiskImage::writeBlocks(DILong index, const DIChar *buf, DIInt num)
{
    return false;
}
