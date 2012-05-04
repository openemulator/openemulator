
/**
 * libdiskimage
 * QCOW Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a QCOW block storage
 */

#include "DIQCOWBlockStorage.h"

DIQCOWBlockStorage::DIQCOWBlockStorage()
{
    close();
}

bool DIQCOWBlockStorage::open(DIBackingStore *backingStore)
{
    close();
    
    // To-Do
    
    return false;
}

void DIQCOWBlockStorage::close()
{
}

bool DIQCOWBlockStorage::isWriteEnabled()
{
    return false;
}

DIInt DIQCOWBlockStorage::getBlockNum()
{
    return 0;
}

string DIQCOWBlockStorage::getFormatLabel()
{
    string formatLabel = "QCOW Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

bool DIQCOWBlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    // To-Do
    
    return false;
}

bool DIQCOWBlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    // To-Do
    
    return false;
}
