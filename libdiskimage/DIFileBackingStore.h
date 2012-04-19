
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

#include <fstream>

#include "DIBackingStore.h"

class DIFileBackingStore : public DIBackingStore
{
public:
    DIFileBackingStore();
    
    bool open(string path);
    void close();
    
    bool isWriteEnabled();
    DILong getSize();
    
    bool read(DILong pos, DIChar *buf, DILong num);
    bool write(DILong pos, const DIChar *buf, DILong num);
    
private:
    fstream file;
    ifstream ifile;
    
    DILong dataSize;
    
    void init();
};

#endif
