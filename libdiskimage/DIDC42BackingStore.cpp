
/**
 * libdiskimage
 * Disk Copy 4.2 Backing Store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a Disk Copy 4.2 backing store
 */

#include "DIDC42BackingStore.h"

#define HEADER_SIZE 0x54

DIDC42BackingStore::DIDC42BackingStore()
{
    close();
}

bool DIDC42BackingStore::open(DIBackingStore *backingStore)
{
    close();
    
    DIChar header[HEADER_SIZE];
    
    if (!backingStore->read(0, header, HEADER_SIZE))
        return false;
    
    // Get image and tag location
    imageOffset = HEADER_SIZE;
    imageSize = getDIIntBE(&header[0x40]);
    
    tagOffset = imageOffset + imageSize;
    tagSize = getDIIntBE(&header[0x44]);
    
    if ((tagOffset + tagSize) != backingStore->getSize())
        return false;
    
    // Get GCR Format byte
    gcrFormat = header[0x51];
    
    // Check id
    if (getDIShortBE(&header[0x52]) != 0x0100)
        return false;
    
    this->backingStore = backingStore;
    
    return true;
}

void DIDC42BackingStore::close()
{
    backingStore = NULL;
    
    gcrFormat = 0;
    
    imageOffset = 0;
    imageSize = 0;
    
    tagOffset = 0;
    tagSize = 0;
}

bool DIDC42BackingStore::isWriteEnabled()
{
    return (backingStore && backingStore->isWriteEnabled());
}

DILong DIDC42BackingStore::getSize()
{
    return imageSize;
}

string DIDC42BackingStore::getFormatLabel()
{
    string formatLabel = "Disk Copy 4.2 Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

DIInt DIDC42BackingStore::getGCRFormat()
{
    return gcrFormat;
}

bool DIDC42BackingStore::read(DILong pos, DIChar *buf, DILong num)
{
    if ((pos + num) > imageSize)
        return false;
    
    return backingStore->read(imageOffset + pos, buf, num);
}

bool DIDC42BackingStore::write(DILong pos, const DIChar *buf, DILong num)
{
    if ((pos + num) > imageSize)
        return false;
    
    return backingStore->write(imageOffset + pos, buf, num);
}

bool DIDC42BackingStore::readTag(DILong pos, DIChar *buf, DILong num)
{
    if ((pos + num) > tagSize)
        return false;
    
    return backingStore->read(tagOffset + pos, buf, num);
}

bool DIDC42BackingStore::writeTag(DILong pos, const DIChar *buf, DILong num)
{
    if ((pos + num) > tagSize)
        return false;
    
    return backingStore->write(tagOffset + pos, buf, num);
}
