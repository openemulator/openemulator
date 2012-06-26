
/**
 * libdiskimage
 * ATA Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an ATA block storage
 */

#include "DIFileBackingStore.h"
#include "DIRAMBackingStore.h"
#include "DIATABlockStorage.h"

DIATABlockStorage::DIATABlockStorage()
{
    close();
}

bool DIATABlockStorage::open(string path)
{
    close();
    
    if (fileBackingStore.open(path) &&
            open(&fileBackingStore, getDIPathExtension(path)))
    {
        ataModel = getDIFilename(path);
        
        return true;
    }
    
    return false;
}

bool DIATABlockStorage::open(DIData& data)
{
    close();
    
    if (ramBackingStore.open(data) &&
        open(&ramBackingStore, ""))
    {
        ataModel = "Memory Disk Image";
        
        return true;
    }
    
    return false;
}

bool DIATABlockStorage::open(DIBackingStore *backingStore, string pathExtension)
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
            (pathExtension != "dmg") &&
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

bool DIATABlockStorage::isOpen()
{
    return blockStorage != &dummyBlockStorage;
}

void DIATABlockStorage::close()
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

bool DIATABlockStorage::isWriteEnabled()
{
    return blockStorage->isWriteEnabled();
}

DIInt DIATABlockStorage::getBlockNum()
{
    return blockStorage->getBlockNum();
}

string DIATABlockStorage::getFormatLabel()
{
    return blockStorage->getFormatLabel();
}

string DIATABlockStorage::getATASerial()
{
    return ataSerial;
}

string DIATABlockStorage::getATAFirmware()
{
    return ataFirmware;
}

string DIATABlockStorage::getATAModel()
{
    return ataModel;
}

bool DIATABlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return blockStorage->readBlocks(index, buf, num);
}

bool DIATABlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    return blockStorage->writeBlocks(index, buf, num);
}
