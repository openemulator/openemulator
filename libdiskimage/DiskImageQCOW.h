
/**
 * libdiskimage
 * QCOW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses DiskCopy 4.2 disk images
 */

#include "DiskImageFile.h"

class DiskImageQCOW
{
public:
    DiskImageQCOW();
    
    bool open(DiskImageFile *data);
    void close();
    
    DILong getBlockNum();
    
    bool readBlocks(DILong index, DIChar *buf, DIInt num);
    bool writeBlocks(DILong index, const DIChar *buf, DIInt num);
    
private:
    DiskImageFile *file;
};
