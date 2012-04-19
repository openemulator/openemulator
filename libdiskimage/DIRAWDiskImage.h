
/**
 * libdiskimage
 * RAW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a RAW disk image
 */

#ifndef _DIRAWDISKIMAGE_H
#define _DIRAWDISKIMAGE_H

#include "DIBackingStore.h"
#include "DIDiskImage.h"

class DIRAWDiskImage : public DIDiskImage
{
public:
    DIRAWDiskImage();
    
    bool open(DIBackingStore *backingStore);
    void close();
    
    bool isWriteEnabled();
    DILong getBlockNum();
    string getFormatLabel();
    
    bool readBlocks(DILong index, DIChar *buf, DIInt num);
    bool writeBlocks(DILong index, const DIChar *buf, DIInt num);
    
private:
    DIBackingStore dummyBackingStore;
    
    DIBackingStore *backingStore;
};

#endif
