
/**
 * libdiskimage
 * Apple Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple block storage
 */

#ifndef _DIAPPLEBLOCKSTORAGE_H
#define _DIAPPLEBLOCKSTORAGE_H

#include "DIFileBackingStore.h"
#include "DIRAMBackingStore.h"
#include "DI2IMGBackingStore.h"
#include "DIDC42BackingStore.h"

#include "DIBlockStorage.h"
#include "DIRAWBlockStorage.h"
#include "DIQCOWBlockStorage.h"
#include "DIVMDKBlockStorage.h"

class DIAppleBlockStorage
{
public:
    DIAppleBlockStorage();
    
    bool open(string path);
    bool open(DIData& data);
    bool isOpen();
    void close();
    
    bool isWriteEnabled();
    DIInt getBlockNum();
    string getFormatLabel();
    
    bool readBlocks(DIInt index, DIChar *buf, DIInt num);
    bool writeBlocks(DIInt index, const DIChar *buf, DIInt num);
    
private:
    DIFileBackingStore fileBackingStore;
    DIRAMBackingStore ramBackingStore;
    DI2IMGBackingStore twoImgBackingStore;
    DIDC42BackingStore dc42BackingStore;
    
    DIBlockStorage dummyBlockStorage;
    DIRAWBlockStorage rawBlockStorage;
    DIQCOWBlockStorage qcowBlockStorage;
    DIVMDKBlockStorage vmdkBlockStorage;
    
    DIBlockStorage *blockStorage;
    
    bool open(DIBackingStore *backingStore, string pathExtension);
};

#endif
