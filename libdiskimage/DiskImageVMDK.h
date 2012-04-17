
/**
 * libdiskimage
 * VMDK Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses VMDK disk images
 */

#include "DiskImageFile.h"

class DiskImageVMDK
{
public:
    DiskImageVMDKDiskImageVMDK();
    
    bool open(DiskImageFile *data);
    void close();
    
    DILong getBlockNum();
    
    bool readBlocks(DILong index, DIChar *buf, DIInt num);
    bool writeBlocks(DILong index, const DIChar *buf, DIInt num);
    
private:
    DiskImageFile *file;
};
