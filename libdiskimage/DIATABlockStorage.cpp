
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
    forceWriteProtected = false;
    maxSize = 0;
    
    close();
}

DIATABlockStorage::~DIATABlockStorage()
{
    close();
}

bool DIATABlockStorage::open(string path)
{
    close();
    
    if (fileBackingStore.open(path) && open(&fileBackingStore))
    {
        model = getLastPathComponent(path);
        
        return true;
    }
    
    return false;
}

bool DIATABlockStorage::open(DIData& data)
{
    close();
    
    if (ramBackingStore.open(data) && open(&ramBackingStore))
    {
        model = "Memory Disk Image";
        
        return true;
    }
    
    return false;
}

bool DIATABlockStorage::open(DIBackingStore *backingStore)
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
        string pathExtension = getPathExtension(fileBackingStore.getPath());
        
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

string DIATABlockStorage::getPath()
{
    return fileBackingStore.getPath();
}

bool DIATABlockStorage::isWriteEnabled()
{
    return !forceWriteProtected && blockStorage->isWriteEnabled();
}

DIInt DIATABlockStorage::getBlockNum()
{
    DIInt blockNum = blockStorage->getBlockNum();
    
    if (!maxSize)
        return blockNum;
    
    return blockNum > maxSize ? maxSize : blockNum;
}

string DIATABlockStorage::getFormatLabel()
{
    return blockStorage->getFormatLabel();
}

void DIATABlockStorage::setForceWriteProtected(bool value)
{
    forceWriteProtected = value;
}

bool DIATABlockStorage::getForceWriteProtected()
{
    return forceWriteProtected;
}

DIInt DIATABlockStorage::getCylinders()
{
    return blockStorage->getCylinders();
}

DIInt DIATABlockStorage::getHeads()
{
    return blockStorage->getHeads();
}

DIInt DIATABlockStorage::getSectors()
{
    return blockStorage->getSectors();
}

string DIATABlockStorage::getSerial()
{
    return "";
}

string DIATABlockStorage::getFirmware()
{
    return "ldi" DI_VERSION;
}

string DIATABlockStorage::getModel()
{
    return model;
}

void DIATABlockStorage::setMaxSize(DIInt value)
{
    maxSize = value;
}

bool DIATABlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return blockStorage->readBlocks(index, buf, num);
}

bool DIATABlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    return blockStorage->writeBlocks(index, buf, num);
}
