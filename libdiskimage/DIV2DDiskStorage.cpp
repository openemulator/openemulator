
/**
 * libdiskimage
 * Disk Image V2D
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses D5NI disk images
 */

#include "DIV2DDiskStorage.h"

#define V2D_SIGNATURE           0x44354e49
#define V2D_HEADER_SIZE         10
#define V2D_TRACKHEADER_SIZE    4

#define V2D_HEADNUM             1
#define V2D_TRACKSPERINCH       192
#define V2D_ROTATIONSPEED       300

#define V2D_TRACKNUM            (40 * 4)
#define V2D_BITRATE             250000

DIV2DDiskStorage::DIV2DDiskStorage()
{
    close();
}

bool DIV2DDiskStorage::open(DIBackingStore *backingStore)
{
    close();
    
    DIChar header[V2D_HEADER_SIZE];
    
    if (!backingStore->read(0, header, V2D_HEADER_SIZE))
        return false;
    
    // Check container size
    DIInt containerByteNum = getDIIntBE(&header[0x00]);
    
    if ((containerByteNum + 8) != backingStore->getSize())
        return false;
    
    // Check id
    if (getDIIntBE(&header[0x04]) != V2D_SIGNATURE)
        return false;
    
    DIInt trackNum = getDIShortBE(&header[0x08]);
    
    DIInt offset = V2D_HEADER_SIZE;
    for (int i = 0; i < trackNum; i++)
    {
        DIChar trackHeader[V2D_TRACKHEADER_SIZE];
        
        if (!backingStore->read(offset, trackHeader, V2D_TRACKHEADER_SIZE))
            return false;
        
        DIInt track = getDIShortBE(&trackHeader[0x00]);
        DIInt size = getDIShortBE(&trackHeader[0x02]);
        
        if (track > V2D_TRACKNUM)
            return false;
        
        offset += V2D_TRACKHEADER_SIZE;
        
        trackOffset[track] = offset;
        trackSize[track] = size;
        
        offset += size;
    }
    
    this->backingStore = backingStore;
    
    return true;
}

void DIV2DDiskStorage::close()
{
    backingStore = NULL;
    
    trackOffset.resize(0);
    trackSize.resize(0);
}

bool DIV2DDiskStorage::isWriteEnabled()
{
    return backingStore->isWriteEnabled();
}

DIDiskType DIV2DDiskStorage::getDiskType()
{
    return DI_525_INCH;
}

DIInt DIV2DDiskStorage::getHeadNum()
{
    return V2D_HEADNUM;
}

float DIV2DDiskStorage::getRotationSpeed()
{
    return V2D_ROTATIONSPEED;
}

DIInt DIV2DDiskStorage::getTracksPerInch()
{
    return V2D_TRACKSPERINCH;
}

string DIV2DDiskStorage::getFormatLabel()
{
    return "V2D Disk Image";
}

bool DIV2DDiskStorage::readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    if (trackIndex >= V2D_TRACKNUM)
        return false;
    
    DIInt size = trackSize[trackIndex];
    
    track.data.resize(size);
    
    if (size)
    {
        track.format = DI_APPLE_NIB;
        
        return backingStore->read(trackOffset[trackIndex], &track.data.front(), size);
    }
    else
    {
        track.format = DI_BLANK;
        
        return true;
    }
}
