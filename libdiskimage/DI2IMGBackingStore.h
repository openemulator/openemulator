
/**
 * libdiskimage
 * 2IMG backing store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a 2IMG backing store
 */

#ifndef _DI2IMGBACKINGSTORE_H
#define _DI2IMGBACKINGSTORE_H

#include "DIBackingStore.h"

class DI2IMGBackingStore : public DIBackingStore
{
public:
    DI2IMGBackingStore();
    
    bool open(DIBackingStore *file);
    void close();
    
    bool isWriteEnabled();
    DILong getSize();
    string getFormatLabel();
    
    string getSectorOrder();
    DIInt getGCRVolume();
    
    bool read(DILong pos, DIChar *buf, DILong num);
    bool write(DILong pos, const DIChar *buf, DILong num);
    
private:
    DIBackingStore *backingStore;
    
    bool writeEnabled;
    string sectorOrder;
    DIInt gcrVolume;
    DILong imageOffset;
    DILong imageSize;
};

#endif
