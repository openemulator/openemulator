
/**
 * libdiskimage
 * Bit Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a data type for representing a bit disk image
 */

#ifndef _BITDISKIMAGE_H
#define _BITDISKIMAGE_H

#include "BlockDiskImage.h"

typedef struct
{
    DIData data;
    int bitnum;
} DITrack;

class BitDiskImage
{
public:
    BitDiskImage();
    BitDiskImage(string path);
    BitDiskImage(DIData& data, string extension);
    ~BitDiskImage();
    
    bool open(string path);
    bool open(DIData& data, string extension);
    bool is_open();
    void close();
    
    bool setProperty(string name, string value);
    bool getProperty(string name, string& value);
    
    bool read(int track, int head, DITrack& data);
    bool write(int track, int head, DITrack& data);
};

#endif
