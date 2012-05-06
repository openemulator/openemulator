
/**
 * libdiskimage
 * VDI Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a VDI block storage
 */

#include "DIVDIBlockStorage.h"

DIVDIBlockStorage::DIVDIBlockStorage()
{
    close();
}

bool DIVDIBlockStorage::open(DIBackingStore *backingStore)
{
    close();
    
    // To-Do
    
    return false;
}

void DIVDIBlockStorage::close()
{
}

bool DIVDIBlockStorage::isWriteEnabled()
{
    return false;
}

DIInt DIVDIBlockStorage::getBlockNum()
{
    return 0;
}

string DIVDIBlockStorage::getFormatLabel()
{
    string formatLabel = "VDI Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

bool DIVDIBlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    // To-Do
    
    return false;
}

bool DIVDIBlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    // To-Do
    
    return false;
}
