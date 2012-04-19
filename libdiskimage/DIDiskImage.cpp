
/**
 * libdiskimage
 * Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the disk image interface
 */

#include "DIDiskImage.h"

DIDiskImage::DIDiskImage()
{
}

bool DIDiskImage::isWriteEnabled()
{
    return false;
}

DILong DIDiskImage::getBlockNum()
{
    return 0;
}

string DIDiskImage::getFormatLabel()
{
    return "";
}

bool DIDiskImage::readBlocks(DILong index, DIChar *buf, DIInt num)
{
    return false;
}

bool DIDiskImage::writeBlocks(DILong index, const DIChar *buf, DIInt num)
{
    return false;
}
