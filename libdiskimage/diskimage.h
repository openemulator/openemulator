
/**
 * libdiskimage
 * Disk image library
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation disk image library
 */

#ifndef _DISKIMAGE_H
#define _DISKIMAGE_H

class DiskImage
{
public:
    DiskImage();
    ~DiskImage();
    
    bool open(string path);
    void close();
    
    bool setInfo(string name, string value);
    bool getInfo(string name, string& value);
    
    bool setBlock(long offset, unsigned char *data, long size);
    bool getBlock(long int offset, unsigned char *data, long& size);
    
    bool setApple525Track();
    bool getApple525Track();

    bool setApple35Track();
    bool getApple35Track();
};

#endif
