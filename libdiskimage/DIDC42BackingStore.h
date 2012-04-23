
/**
 * libdiskimage
 * Disk Copy 4.2 Backing Store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a Disk Copy 4.2 backing store
 */

#ifndef _DIDC42BACKINGSTORE_H
#define _DIDC42BACKINGSTORE_H

#include "DICommon.h"
#include "DIBackingStore.h"

#define DI_DC42_TAGSIZE   12

class DIDC42BackingStore : public DIBackingStore
{
public:
    DIDC42BackingStore();
    
    bool open(DIBackingStore *backingStore);
    void close();
    
    bool isWriteEnabled();
    DILong getSize();
    string getFormatLabel();
    
    DIInt getGCRFormat();
    
    bool read(DILong pos, DIChar *buf, DIInt num);
    bool write(DILong pos, const DIChar *buf, DIInt num);
    
    bool readTag(DILong pos, DIChar *buf, DIInt num);
    bool writeTag(DILong pos, const DIChar *buf, DIInt num);
    
private:
    DIBackingStore *backingStore;
    
    DIInt gcrFormat;
    
    DILong imageOffset;
    DILong imageSize;
    
    DILong tagOffset;
    DILong tagSize;
};

#endif
