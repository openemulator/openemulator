
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

#include "DICommon.h"
#include "DIBackingStore.h"

class DIFileBackingStore : public DIBackingStore
{
public:
    DIFileBackingStore();
    ~DIFileBackingStore();
    
    bool open(string path);
    bool create(string path);
    void close();
    
    string getPath();
    bool isWriteEnabled();
    DILong getSize();
    string getFormatLabel();
    
    bool read(DILong pos, DIChar *buf, DIInt num);
    bool write(DILong pos, const DIChar *buf, DIInt num);
    
private:
    FILE *fp;
    bool writeEnabled;
    
    string path;
};

#endif
