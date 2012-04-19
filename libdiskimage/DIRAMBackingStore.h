
/**
 * libdiskimage
 * RAM Backing Store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a RAM backing store
 */

#ifndef _DIRAMBACKINGSTORE_H
#define _DIRAMBACKINGSTORE_H

#include <fstream>

#include "DIBackingStore.h"

class DIRAMBackingStore : public DIBackingStore
{
public:
    DIRAMBackingStore();
    
    bool open(DIData& data);
    void close();
    
    bool isWriteEnabled();
    DILong getSize();
    
    bool read(DILong pos, DIChar *buf, DILong num);
    bool write(DILong pos, const DIChar *buf, DILong num);
    
private:
    DIData data;
    
    void init();
};

#endif
