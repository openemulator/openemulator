
/**
 * libdiskimage
 * File Backing Store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a file backing store
 */
 
#ifndef _DIFILEBACKINGSTORE_H
#define _DIFILEBACKINGSTORE_H

#include "stdio.h"

#include "DIBackingStore.h"

class DIFileBackingStore : public DIBackingStore
{
public:
    DIFileBackingStore();
    
    bool open(string path);
    void close();
    
    bool isWriteEnabled();
    DILong getSize();
    
    bool read(DILong pos, DIChar *buf, DIInt num);
    bool write(DILong pos, const DIChar *buf, DIInt num);
    
    bool truncate();
    
private:
    FILE *fp;
    bool writeEnabled;
    
    string path;
};

#endif
