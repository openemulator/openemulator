
/**
 * libdiskimage
 * Disk Image V2D
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses Virtual II disk images
 */

#include "DiskImageV2D.h"

#define HEADER_SIZE 10
#define TRACKHEADER_SIZE 4

DiskImageV2D::DiskImageV2D()
{
    close();
}

bool DiskImageV2D::open(DiskImageFile *file)
{
    this->file = NULL;
    
    DIChar header[HEADER_SIZE];
    
    if (!file->read(0, header, HEADER_SIZE))
        return false;
    
    // Check container size
    DIInt containerByteNum = getDIIntBE(&header[0x00]);
    
    if ((containerByteNum + 8) != file->getSize())
        return false;
    
    // Check id
    if (getDIIntBE(&header[0x04]) != 0x44354e49)
        return false;
    
    DIInt trackNum = getDIShortBE(&header[0x08]);
    
    DIInt trackOffset = HEADER_SIZE;
    
    for (int i = 0; i < trackNum; i++)
    {
        DIChar trackHeader[TRACKHEADER_SIZE];
        
        if (!file->read(trackOffset, trackHeader, TRACKHEADER_SIZE))
            return false;
        
        DIInt quarterTrackIndex = getDIIntBE(&trackHeader[0x00]);
        DIInt trackSize = getDIShortBE(&trackHeader[0x02]);
        
//        tracks[quarterTrackIndex].bit
        
//        if (!data->read(trackOffset + TRACKHEADER_SIZE, 
//                        imageOffset + pos, buf, num);
    }
    
    this->file = file;
    
    return true;
}

void DiskImageV2D::close()
{
    file = NULL;
    
    tracks.resize(0);
}

bool DiskImageV2D::readTrack(DIInt quarterTrackIndex, DIData& nibbleData)
{
    return false;
}
