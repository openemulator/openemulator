
/**
 * libdiskimage
 * Disk Image File
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a disk image file
 */
 
#ifndef _DISKIMAGEFILE_H
#define _DISKIMAGEFILE_H

#include <fstream>

#include "DiskImageCommon.h"

class DiskImageFile
{
public:
    DiskImageFile();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool isReadOnly();
    DILong getSize();
    
    bool read(DILong pos, DIChar *buf, DILong num);
    bool write(DILong pos, const DIChar *buf, DILong num);
    
private:
    fstream file;
    ifstream ifile;
    DIData data;
    
    DILong dataSize;
    
    void init();
};

#endif
