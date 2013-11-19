
/**
 * libdiskimage
 * RAW Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a RAW block storage
 */

#include "DIRAWBlockStorage.h"

DIRAWBlockStorage::DIRAWBlockStorage()
{
    close();
}

bool DIRAWBlockStorage::open(DIBackingStore *backingStore)
{
    close();
    
    // Check size for multiple of DI_BLOCKSIZE
    if (backingStore->getSize() % DI_BLOCKSIZE)
        return false;
    
    this->backingStore = backingStore;
    
    blockNum = (DIInt) (backingStore->getSize() / DI_BLOCKSIZE);
    
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
    return blockNum;
}

string DIRAWBlockStorage::getFormatLabel()
{
    return backingStore->getFormatLabel();
}

bool DIRAWBlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return backingStore->read(index * DI_BLOCKSIZE, buf, num * DI_BLOCKSIZE);
}

bool DIRAWBlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    if ((index + num) > blockNum)
        return false;
    
    return backingStore->write(index * DI_BLOCKSIZE, buf, num * DI_BLOCKSIZE);
}
