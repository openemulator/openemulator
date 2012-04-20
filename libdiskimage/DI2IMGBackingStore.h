
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

typedef enum
{
    DI_2IMG_DOS,
    DI_2IMG_PRODOS,
    DI_2IMG_NIB,
} DI2IMGFormat;

class DI2IMGBackingStore : public DIBackingStore
{
public:
    DI2IMGBackingStore();
    
    bool open(DIBackingStore *file);
    void close();
    
    bool isWriteEnabled();
    DILong getSize();
    string getFormatLabel();
    
    DI2IMGFormat getFormat();
    DIInt getGCRVolume();
    
    bool read(DILong pos, DIChar *buf, DIInt num);
    bool write(DILong pos, const DIChar *buf, DIInt num);
    
private:
    DIBackingStore *backingStore;
    
    bool writeEnabled;
    DI2IMGFormat format;
    DIInt gcrVolume;
    DILong imageOffset;
    DILong imageSize;
};

#endif
