
/**
 * libdiskimage
 * RAW Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a RAW block storage
 */

#ifndef _DIRAWBLOCKSTORAGE_H
#define _DIRAWBLOCKSTORAGE_H

#include "DICommon.h"
#include "DIBackingStore.h"
#include "DIBlockStorage.h"

class DIRAWBlockStorage : public DIBlockStorage
{
public:
    DIRAWBlockStorage();
    
    bool open(DIBackingStore *backingStore);
    void close();
    
    bool isWriteEnabled();
    DIInt getBlockNum();
    string getFormatLabel();
    
    bool readBlocks(DIInt index, DIChar *buf, DIInt num);
    bool writeBlocks(DIInt index, const DIChar *buf, DIInt num);
    
private:
    DIBackingStore *backingStore;
    
    DIInt blockNum;
};

#endif
