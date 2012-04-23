
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
    close();
    
    return false;
}

void DIVMDKDiskImage::close()
{
}

bool DIVMDKDiskImage::isWriteEnabled()
{
    return false;
}

DIInt DIVMDKDiskImage::getBlockNum()
{
    return 0;
}

string DIVMDKDiskImage::getFormatLabel()
{
    string formatLabel = "VMDK Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

bool DIVMDKDiskImage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return false;
}

bool DIVMDKDiskImage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    return false;
}
