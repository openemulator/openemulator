
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

#include "DICommon.h"
#include "DIBackingStore.h"

class DIRAMBackingStore : public DIBackingStore
{
public:
    DIRAMBackingStore();
    
    bool open(DIData& data);
    void create();
    void close();
    
    bool isWriteEnabled();
    DILong getSize();
    string getFormatLabel();
    
    bool read(DILong pos, DIChar *buf, DIInt num);
    bool write(DILong pos, const DIChar *buf, DIInt num);
    
private:
    DIData data;
};

#endif
