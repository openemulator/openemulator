
/**
 * libdiskimage
 * QCOW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses QCOW disk images
 */

#include "DIQCOWDiskImage.h"

DIQCOWDiskImage::DIQCOWDiskImage()
{
    close();
}

bool DIQCOWDiskImage::open(DIBackingStore *backingStore)
{
    close();
    
    return false;
}

void DIQCOWDiskImage::close()
{
}

bool DIQCOWDiskImage::isWriteEnabled()
{
    return false;
}

DIInt DIQCOWDiskImage::getBlockNum()
{
    return 0;
}

string DIQCOWDiskImage::getFormatLabel()
{
    string formatLabel = "QCOW Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}
    
bool DIQCOWDiskImage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return false;
}

bool DIQCOWDiskImage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    return false;
}
