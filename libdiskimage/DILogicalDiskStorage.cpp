
/**
 * libdiskimage
 * RAW Disk Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a RAW disk image
 */

#include "DILogicalDiskStorage.h"

DILogicalDiskStorage::DILogicalDiskStorage()
{
    close();
}

bool DILogicalDiskStorage::open(DIBackingStore *backingStore,
                                DIDiskType diskType, DIInt headNum,
                                float rotationSpeed, DIInt tracksPerInch,
                                DITrackFormat trackFormat, DIInt trackSize)
{
    close();
    
    this->backingStore = backingStore;
    
    this->diskType = diskType;
    this->headNum = headNum;
    this->rotationSpeed = rotationSpeed;
    this->tracksPerInch = tracksPerInch;
    
    this->trackFormat = trackFormat;
    this->trackSize = trackSize;
    
    return true;
}

void DILogicalDiskStorage::close()
{
    backingStore = NULL;
    
    trackFormat = DI_BLANK;
    trackSize = 0;
}

bool DILogicalDiskStorage::isWriteEnabled()
{
    return backingStore->isWriteEnabled();
}

DIDiskType DILogicalDiskStorage::getDiskType()
{
    return diskType;
}

DIInt DILogicalDiskStorage::getHeadNum()
{
    return headNum;
}

float DILogicalDiskStorage::getRotationSpeed()
{
    return rotationSpeed;
}

DIInt DILogicalDiskStorage::getTracksPerInch()
{
    return tracksPerInch;
}

string DILogicalDiskStorage::getFormatLabel()
{
    return backingStore->getFormatLabel();
}

DITrackFormat DILogicalDiskStorage::getTrackFormat()
{
    return trackFormat;
}

bool DILogicalDiskStorage::readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    track.data.resize(trackSize);
    track.format = trackFormat;
    
    DIInt index = trackIndex * headNum + headIndex;
    return backingStore->read(trackSize * index, &track.data.front(), trackSize);
}

bool DILogicalDiskStorage::writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    track.data.resize(trackSize);
    
    DIInt index = trackIndex * headNum + headIndex;
    return backingStore->write(trackSize * index, &track.data.front(), trackSize);
}
