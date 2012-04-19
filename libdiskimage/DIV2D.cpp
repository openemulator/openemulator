
/**
 * libdiskimage
 * Disk Image V2D
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses Virtual II disk images
 */

#include "DIV2D.h"

#define HEADER_SIZE 10
#define TRACKHEADER_SIZE 4

DiskImageV2D::DiskImageV2D()
{
    close();
}

bool DiskImageV2D::open(DiskImageFile *file)
{
    close();
    
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
    
    DIInt offset = HEADER_SIZE;
    for (int i = 0; i < trackNum; i++)
    {
        DIChar trackHeader[TRACKHEADER_SIZE];
        
        if (!file->read(offset, trackHeader, TRACKHEADER_SIZE))
            return false;
        
        DIInt index = getDIShortBE(&trackHeader[0x00]);
        DIInt size = getDIShortBE(&trackHeader[0x02]);
        
        trackOffset[index] = offset;
        trackSize[index] = size;
        
        offset += TRACKHEADER_SIZE + size;
    }
    
    this->file = file;
    
    return true;
}

void DiskImageV2D::close()
{
    file = NULL;
    
    trackOffset.resize(0);
    trackSize.resize(0);
}

bool DiskImageV2D::readTrack(DIInt track, DIData& buf)
{
    if (!file)
        return false;
    
    buf.resize(trackSize[track]);
    
    return file->read(trackOffset[track], &buf.front(), trackSize[track]);
}
