
/**
 * libdiskimage
 * Block Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a data type for representing a block disk image
 */

#include "BlockDiskImage.h"

BlockDiskImage::BlockDiskImage()
{
}

BlockDiskImage::BlockDiskImage(string path)
{
}

BlockDiskImage::BlockDiskImage(DIData& data, string extension)
{
}

BlockDiskImage::~BlockDiskImage()
{
}

bool BlockDiskImage::open(string path)
{
    return true;
}

bool BlockDiskImage::open(DIData &data, string extension)
{
    return true;
}

bool BlockDiskImage::is_open()
{
    return true;
}

void BlockDiskImage::close()
{
}

bool BlockDiskImage::setProperty(string name, string value)
{
    return false;
}

bool BlockDiskImage::getProperty(string name, string &value)
{
    return false;
}

bool BlockDiskImage::read(DILong offset, DIData& data)
{
    return false;
}

bool BlockDiskImage::write(DILong offset, DIData& data)
{
    return false;
}

bool BlockDiskImage::readTag(DILong offset, DIData& data)
{
    return false;
}

bool BlockDiskImage::writeTag(DILong offset, DIData& data)
{
    return false;
}
