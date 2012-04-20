
/**
 * libdiskimage
 * Backing Store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the backing store interface
 */

#ifndef _DIBACKINGSTORE_H
#define _DIBACKINGSTORE_H

#include "DICommon.h"

class DIBackingStore
{
public:
    DIBackingStore();
    
    virtual bool isWriteEnabled();
    virtual DILong getSize();
    virtual string getFormatLabel();
    
    virtual bool read(DILong pos, DIChar *buf, DIInt num);
    virtual bool write(DILong pos, const DIChar *buf, DIInt num);
    
    virtual bool truncate();
};

#endif
