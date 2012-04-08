
/**
 * libdiskimage
 * Block Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a data type for representing a block disk image
 */

#ifndef _BLOCKDISKIMAGE_H
#define _BLOCKDISKIMAGE_H

#include "DICommon.h"

using namespace std;

class BlockDiskImage
{
public:
    BlockDiskImage();
    BlockDiskImage(string path);
    BlockDiskImage(DIData& data, string extension);
    virtual ~BlockDiskImage();
    
    virtual bool open(string path);
    virtual bool open(DIData& data, string extension);
    virtual bool is_open();
    virtual void close();
    
    virtual bool setProperty(string name, string value);
    virtual bool getProperty(string name, string& value);
    
    virtual bool read(DILong offset, DIData& data);
    virtual bool write(DILong offset, DIData& data);
    
    virtual bool readTag(DILong offset, DIData& data);
    virtual bool writeTag(DILong offset, DIData& data);
};

#endif
