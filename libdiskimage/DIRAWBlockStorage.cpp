
/**
 * libdiskimage
 * RAW Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a RAW block storage
 */

#include "DIRAWBlockStorage.h"

#define BLOCK_SIZE 512

DIRAWBlockStorage::DIRAWBlockStorage()
{
    close();
}

bool DIRAWBlockStorage::open(DIBackingStore *backingStore)
{
    close();
    
    // Check size for multiple of BLOCK_SIZE
    if (backingStore->getSize() % BLOCK_SIZE)
        return false;
    
    this->backingStore = backingStore;
    
    return true;
}

void DIRAWBlockStorage::close()
{
    backingStore = NULL;
}

bool DIRAWBlockStorage::isWriteEnabled()
{
    return backingStore->isWriteEnabled();
}

DIInt DIRAWBlockStorage::getBlockNum()
{
    return (DIInt) (backingStore->getSize() / BLOCK_SIZE);
}

string DIRAWBlockStorage::getFormatLabel()
{
    return backingStore->getFormatLabel();
}

bool DIRAWBlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return backingStore->read(index * BLOCK_SIZE, buf, num * BLOCK_SIZE);
}

bool DIRAWBlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    return backingStore->write(index * BLOCK_SIZE, buf, num * BLOCK_SIZE);
}
