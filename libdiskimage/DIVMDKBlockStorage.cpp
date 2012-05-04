
/**
 * libdiskimage
 * VMDK Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a VMDK block storage
 */

#include "DIVMDKBlockStorage.h"

DIVMDKBlockStorage::DIVMDKBlockStorage()
{
    close();
}

bool DIVMDKBlockStorage::open(DIBackingStore *backingStore)
{
    close();
    
    // To-Do
    
    return false;
}

void DIVMDKBlockStorage::close()
{
}

bool DIVMDKBlockStorage::isWriteEnabled()
{
    return false;
}

DIInt DIVMDKBlockStorage::getBlockNum()
{
    return 0;
}

string DIVMDKBlockStorage::getFormatLabel()
{
    string formatLabel = "VMDK Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

bool DIVMDKBlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    // To-Do
    
    return false;
}

bool DIVMDKBlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    // To-Do
    
    return false;
}
