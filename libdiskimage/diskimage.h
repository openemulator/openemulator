
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
} DiskImagePhysicalTrack;

class DiskImage
{
public:
    DiskImage();
    DiskImage(string path);
    DiskImage(DiskImageData& data);
    ~DiskImage();
    
    bool open(string path);
    bool open(DiskImageData& data);
    void close();
    
    bool setProperty(string name, string value);
    bool getProperty(string name, string& value);
    
    bool setBlock(unsigned long offset, DiskImageData& data);
    bool getBlock(unsigned long offset, DiskImageData& data);
    
    bool setRawTrack(int track, int head, DiskImagePhysicalTrack& data);
    bool getRawTrack(int track, int head, DiskImagePhysicalTrack& data);
};

#endif
