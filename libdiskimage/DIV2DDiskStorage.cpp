
/**
 * libdiskimage
 * Disk Image V2D
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses D5NI disk images
 */

#include "DIV2DDiskStorage.h"

#define V2D_TRACKNUM            (40 * 4)
#define V2D_SIGNATURE           0x44354e49
#define V2D_HEADER_SIZE         10
#define V2D_TRACKHEADER_SIZE    4

DIV2DDiskStorage::DIV2DDiskStorage()
{
    backingStore = NULL;
    
    trackData.resize(V2D_TRACKNUM);
}

bool DIV2DDiskStorage::open(DIBackingStore *backingStore)
{
    DIChar header[V2D_HEADER_SIZE];
    
    if (!backingStore->read(0, header, V2D_HEADER_SIZE))
        return false;
    
    // Check container size
    DIInt containerByteNum = getDIIntBE(&header[0x00]);
    
    if ((containerByteNum + 8) != backingStore->getSize())
        return false;
    
    // Check id
    if (getDIIntBE(&header[0x04]) != 0x44354e49)
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
        
        trackData[track].resize(size);
        if (!backingStore->read(offset, &trackData[track].front(), size))
            return false;
        
        offset += size;
    }
    
    this->backingStore = backingStore;
    
    return true;
}

void DIV2DDiskStorage::close()
{
    if (!backingStore)
        return;
    
    backingStore->truncate();
    
    DIInt fileSize = V2D_HEADER_SIZE;
    DIInt trackNum = 0;
    
    for (DIInt track = 0; track < V2D_TRACKNUM; track++)
    {
        if (!trackData[track].size())
            continue;
        
        fileSize += V2D_HEADER_SIZE + trackData[track].size();
        trackNum++;
    }
    
    DIChar header[V2D_HEADER_SIZE];
    
    setDIIntBE(&header[0x00], fileSize);
    setDIIntBE(&header[0x04], V2D_SIGNATURE);
    setDIShortBE(&header[0x08], trackNum);
    
    if (!backingStore->write(0, header, V2D_HEADER_SIZE))
        return;
    
    DIInt offset = V2D_HEADER_SIZE;
    for (int track = 0; track < V2D_TRACKNUM; track++)
    {
        DIInt size = trackData[track].size();
        if (!size)
            continue;
        
        DIChar trackHeader[V2D_TRACKHEADER_SIZE];
        
        setDIShortBE(&trackHeader[0x00], track);
        setDIShortBE(&trackHeader[0x02], size);
        
        if (!backingStore->write(offset, trackHeader, V2D_TRACKHEADER_SIZE))
            return;
        
        offset += V2D_TRACKHEADER_SIZE;
        
        if (!backingStore->write(offset, &trackData[track].front(), size))
            return;
        
        offset += size;
    }
    
    return;
}

bool DIV2DDiskStorage::readTrack(DIInt track, DIData& buf)
{
    if (track >= V2D_TRACKNUM)
        return false;
    
    buf = trackData[track];
    
    return true;
}

bool DIV2DDiskStorage::writeTrack(DIInt track, DIData& buf)
{
    if (track >= V2D_TRACKNUM)
        return false;
    
    trackData[track] = buf;
    
    return true;
}
