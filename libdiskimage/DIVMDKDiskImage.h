
/**
 * libdiskimage
 * VMDK Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses VMDK disk images
 */

#ifndef DIVMDKDISKIMAGE_H
#define DIVMDKDISKIMAGE_H

#include "DICommon.h"
#include "DIBackingStore.h"
#include "DIDiskImage.h"

class DIVMDKDiskImage : public DIDiskImage
{
public:
    DIVMDKDiskImage();
    
    bool open(DIBackingStore *backingStore);
    void close();
    
    bool isWriteEnabled();
    DIInt getBlockNum();
    string getFormatLabel();
    
    bool readBlocks(DIInt index, DIChar *buf, DIInt num);
    bool writeBlocks(DIInt index, const DIChar *buf, DIInt num);
    
private:
    DIBackingStore *backingStore;
};

#endif
