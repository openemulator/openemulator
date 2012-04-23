
/**
 * libdiskimage
 * RAW Disk Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a RAW disk image
 */

#ifndef _DILOGICALDISKSTORAGE_H
#define _DILOGICALDISKSTORAGE_H

#include "DICommon.h"
#include "DIBackingStore.h"
#include "DIDiskStorage.h"

class DILogicalDiskStorage : public DIDiskStorage
{
public:
    DILogicalDiskStorage();
    
    bool open(DIBackingStore *backingStore,
              DIDiskType diskType, DIInt headNum,
              float rotationSpeed, DIInt tracksPerInch,
              DITrackFormat trackFormat, DIInt trackSize);
    void close();
    
    bool isWriteEnabled();
    DIDiskType getDiskType();
    DIInt getHeadNum();
    float getRotationSpeed();
    DIInt getTracksPerInch();
    string getFormatLabel();
    
    bool readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    bool writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    
private:
    DIBackingStore dummyBackingStore;
    
    DIBackingStore *backingStore;
    
    DIDiskType diskType;
    DIInt headNum;
    float rotationSpeed;
    DIInt tracksPerInch;
    
    DITrackFormat trackFormat;
    DIInt trackSize;
};

#endif
