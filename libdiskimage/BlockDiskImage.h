
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

#define DISKIMAGE_BLOCKSIZE  512
#define DISKIMAGE_TAGSIZE    12

class BlockDiskImage
{
public:
    BlockDiskImage();
    BlockDiskImage(string path);
    BlockDiskImage(DIData& data);
    virtual ~BlockDiskImage();
    
    virtual bool open(string path);
    virtual bool open(DIData& data);
    virtual bool is_open();
    virtual void close();
    
    virtual bool setProperty(string name, string value);
    virtual bool getProperty(string name, string& value);
    
    virtual bool read(DILong blockIndex, DIChar *blockData, DIInt blockNum);
    virtual bool write(DILong blockIndex, DIChar *blockData, DIInt blockNum);
    
    virtual bool readTag(DILong tagIndex, DIChar *tagData, DIInt tagNum);
    virtual bool writeTag(DILong tagIndex, DIChar *tagData, DIInt tagNum);
};

#endif
