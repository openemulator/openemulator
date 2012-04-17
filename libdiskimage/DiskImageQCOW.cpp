
/**
 * libdiskimage
 * QCOW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses QCOW disk images
 */

#include "DiskImageQCOW.h"

DiskImageQCOW::DiskImageQCOW()
{
    close();
}

bool DiskImageQCOW::open(DiskImageFile *data)
{
    return false;
}

void DiskImageQCOW::close()
{
}

DILong DiskImageQCOW::getBlockNum()
{
    return 0;
}
    
bool DiskImageQCOW::readBlocks(DILong index, DIChar *buf, DIInt num)
{
    return false;
}

bool DiskImageQCOW::writeBlocks(DILong index, const DIChar *buf, DIInt num)
{
    return false;
}
