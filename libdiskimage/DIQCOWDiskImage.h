
/**
 * libdiskimage
 * QCOW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses QCOW disk images
 */

#ifndef DIQCOWDISKIMAGE_H
#define DIQCOWDISKIMAGE_H

#include "DICommon.h"
#include "DIBackingStore.h"
#include "DIDiskImage.h"

class DIQCOWDiskImage : public DIDiskImage
{
public:
    DIQCOWDiskImage();
    
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
