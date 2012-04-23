
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

DIInt DIDiskImage::getBlockNum()
{
    return 0;
}

string DIDiskImage::getFormatLabel()
{
    return "";
}

bool DIDiskImage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return false;
}

bool DIDiskImage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    return false;
}
