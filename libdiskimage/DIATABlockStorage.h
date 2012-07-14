
/**
 * libdiskimage
 * ATA Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an ATA block storage
 */

#ifndef _DIATABLOCKSTORAGE_H
#define _DIATABLOCKSTORAGE_H

#include "DIFileBackingStore.h"
#include "DIRAMBackingStore.h"
#include "DI2IMGBackingStore.h"
#include "DIDC42BackingStore.h"

#include "DIBlockStorage.h"
#include "DIRAWBlockStorage.h"
#include "DIVDIBlockStorage.h"
#include "DIVMDKBlockStorage.h"

class DIATABlockStorage
{
public:
    DIATABlockStorage();
    ~DIATABlockStorage();
    
    bool open(string path);
    bool open(DIData& data);
    bool isOpen();
    void close();
    
    string getPath();
    bool isWriteEnabled();
    DIInt getBlockNum();
    string getFormatLabel();
    
    void setForceWriteProtected(bool value);
    bool getForceWriteProtected();
    
    DIInt getCylinders();
    DIInt getHeads();
    DIInt getSectors();
    
    string getSerial();
    string getFirmware();
    string getModel();
    
    void setMaxSize(DIInt value);
    
    bool readBlocks(DIInt index, DIChar *buf, DIInt num);
    bool writeBlocks(DIInt index, const DIChar *buf, DIInt num);
    
private:
    DIFileBackingStore fileBackingStore;
    DIRAMBackingStore ramBackingStore;
    DI2IMGBackingStore twoImgBackingStore;
    DIDC42BackingStore dc42BackingStore;
    
    DIBlockStorage dummyBlockStorage;
    DIRAWBlockStorage rawBlockStorage;
    DIVDIBlockStorage vdiBlockStorage;
    DIVMDKBlockStorage vmdkBlockStorage;
    
    DIBlockStorage *blockStorage;
    
    bool forceWriteProtected;
    string model;
    
    DIInt maxSize;
    
    bool open(DIBackingStore *backingStore);
};

#endif
