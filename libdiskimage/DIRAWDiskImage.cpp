
/**
 * libdiskimage
 * RAW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a RAW disk image
 */

#include "DIRAWDiskImage.h"

#define BLOCK_SIZE 512

DIRAWDiskImage::DIRAWDiskImage()
{
    close();
}

bool DIRAWDiskImage::open(DIBackingStore *backingStore)
{
    close();
    
    // Check size for multiple of BLOCK_SIZE
    if (backingStore->getSize() % BLOCK_SIZE)
        return false;
    
    this->backingStore = backingStore;
    
    return true;
}

void DIRAWDiskImage::close()
{
    backingStore = NULL;
}

bool DIRAWDiskImage::isWriteEnabled()
{
    return backingStore->isWriteEnabled();
}

DIInt DIRAWDiskImage::getBlockNum()
{
    return (DIInt) (backingStore->getSize() / BLOCK_SIZE);
}

string DIRAWDiskImage::getFormatLabel()
{
    return backingStore->getFormatLabel();
}

bool DIRAWDiskImage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return backingStore->read(index * BLOCK_SIZE, buf, num * BLOCK_SIZE);
}

bool DIRAWDiskImage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    return backingStore->write(index * BLOCK_SIZE, buf, num * BLOCK_SIZE);
}
