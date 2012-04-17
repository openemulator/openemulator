
/**
 * libdiskimage
 * VMDK Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses VMDK disk images
 */

#include "DiskImageVMDK.h"

DiskImageVMDK::DiskImageVMDK()
{
    close();
}

bool DiskImageVMDK::open(DiskImageFile *data)
{
    return false;
}

void DiskImageVMDK::close()
{
}

DILong DiskImageVMDK::getBlockNum()
{
    return 0;
}

bool DiskImageVMDK::readBlocks(DILong index, DIChar *buf, DIInt num)
{
    return false;
}

bool DiskImageVMDK::writeBlocks(DILong index, const DIChar *buf, DIInt num)
{
    return false;
}
