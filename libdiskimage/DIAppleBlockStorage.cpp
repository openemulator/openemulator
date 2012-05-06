
/**
 * libdiskimage
 * Apple Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple block storage
 */

#include "DIFileBackingStore.h"
#include "DIRAMBackingStore.h"
#include "DIAppleBlockStorage.h"

DIAppleBlockStorage::DIAppleBlockStorage()
{
    close();
}

bool DIAppleBlockStorage::open(string path)
{
    close();
    
    return (fileBackingStore.open(path) &&
            open(&fileBackingStore, getDIPathExtension(path)));
}

bool DIAppleBlockStorage::open(DIData& data)
{
    close();
    
    return (ramBackingStore.open(data) &&
            open(&ramBackingStore, ""));
}

bool DIAppleBlockStorage::open(DIBackingStore *backingStore, string pathExtension)
{
    if (twoImgBackingStore.open(backingStore))
    {
        if (twoImgBackingStore.getFormat() != DI_2IMG_PRODOS)
            return false;
        
        backingStore = &twoImgBackingStore;
    }
    else if (dc42BackingStore.open(backingStore))
        backingStore = &dc42BackingStore;
    else if (vdiBlockStorage.open(backingStore))
    {
        blockStorage = &vdiBlockStorage;
        
        return true;
    }
    else if (vmdkBlockStorage.open(backingStore))
    {
        blockStorage = &vmdkBlockStorage;
        
        return true;
    }
    else
    {
        if ((pathExtension != "image") &&
            (pathExtension != "img") &&
            (pathExtension != "hdf") &&
            (pathExtension != "hdv") &&
            (pathExtension != "po") &&
            (pathExtension != "vdsk"))
            return false;
    }
    
    if (!rawBlockStorage.open(backingStore))
        return false;
    
    blockStorage = &rawBlockStorage;
    
    return true;
}

bool DIAppleBlockStorage::isOpen()
{
    return blockStorage != &dummyBlockStorage;
}

void DIAppleBlockStorage::close()
{
    fileBackingStore.close();
    ramBackingStore.close();
    twoImgBackingStore.close();
    dc42BackingStore.close();
    
    rawBlockStorage.close();
    vdiBlockStorage.close();
    vmdkBlockStorage.close();
    
    blockStorage = &dummyBlockStorage;
    
    return;
}

bool DIAppleBlockStorage::isWriteEnabled()
{
    return blockStorage->isWriteEnabled();
}

DIInt DIAppleBlockStorage::getBlockNum()
{
    return blockStorage->getBlockNum();
}

string DIAppleBlockStorage::getFormatLabel()
{
    return blockStorage->getFormatLabel();
}

bool DIAppleBlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return blockStorage->readBlocks(index, buf, num);
}

bool DIAppleBlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    return blockStorage->writeBlocks(index, buf, num);
}
