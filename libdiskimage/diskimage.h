
/**
 * libdiskimage
 * Disk image library
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an emulation disk image library
 */

/*
 * Standard properties:
 *
 * sectorsPerTrack
 * trackNum
 * headNum
 * readonly
 * lockSupported
 * description
 * creator
 *
 * Apple II properties:
 * 
 */

// Ideas:
// * Data is always opened track by track, except for FDI files
// * When writing a dsk file and not every sector is recognized,
//   the original file is automatically closed, and an .fdi file
//   is created instead
// * When an FDI file is opened or created, everything is loaded
//   to memory

#ifndef _FLOPPYDISK_H
#define _FLOPPYDISK_H

#include <vector>

using namespace std;

typedef vector<unsigned char> DiskImageData;

typedef struct
{
    DiskImageData data;
    int bitnum;
} DiskImageBitData;

class BlockDiskImage
{
public:
    BlockDiskImage();
    BlockDiskImage(string path);
    BlockDiskImage(DiskImageData& data);
    ~BlockDiskImage();
    
    bool open(string path);
    bool open(DiskImageData& data);
    void close();
    
    bool setProperty(string name, string value);
    bool getProperty(string name, string& value);
    
    bool read(unsigned long offset, DiskImageData& data);
    bool write(unsigned long offset, DiskImageData& data);
};

class BitDiskImage
{
public:
    BitDiskImage();
    BitDiskImage(string path);
    BitDiskImage(DiskImageData& data);
    ~BitDiskImage();
    
    bool open(string path);
    bool open(DiskImageData& data);
    void close();
    
    bool setProperty(string name, string value);
    bool getProperty(string name, string& value);
    
    bool read(int track, int head, DiskImageBitData& data);
    bool write(int track, int head, DiskImageBitData& data);
};

#endif
