
/**
 * libdiskimage
 * Disk Image Apple Block
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple block disk
 */

#ifndef _DIAPPLEDISKIMAGE_H
#define _DIAPPLEDISKIMAGE_H

#include "DIBackingStore.h"
#include "DIDiskImage.h"

#include "DIFileBackingStore.h"
#include "DIRAMBackingStore.h"
#include "DI2IMGBackingStore.h"
#include "DIDC42BackingStore.h"
#include "DIRAWDiskImage.h"
#include "DIQCOWDiskImage.h"
#include "DIVMDKDiskImage.h"

class DIAppleDiskImage : DIDiskImage
{
public:
    DIAppleDiskImage();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool isWriteEnabled();
    DILong getBlockNum();
    string getFormatLabel();
    
    bool readBlocks(DILong index, DIChar *buf, DIInt num);
    bool writeBlocks(DILong index, const DIChar *buf, DIInt num);
    
private:
    DIDiskImage dummyDiskImage;
    DIFileBackingStore fileBackingStore;
    DIRAMBackingStore ramBackingStore;
    DI2IMGBackingStore twoImgBackingStore;
    DIDC42BackingStore dc42BackingStore;
    DIRAWDiskImage rawDiskImage;
    DIQCOWDiskImage qcowDiskImage;
    DIVMDKDiskImage vmdkDiskImage;
    
    DIDiskImage *diskImage;
    
    bool open(DIBackingStore *backingStore);
};

#endif
