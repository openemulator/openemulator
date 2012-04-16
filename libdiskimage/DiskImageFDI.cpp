
/**
 * libdiskimage
 * Disk Image FDI
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses FDI disk images
 */

#include "string.h"
#include "DiskImageFDI.h"

#define FDI_SIGNATURE   "Formatted Disk Image File\n\r"

DiskImageFDI::DiskImageFDI()
{
    readOnly = false;
    diskType = DI_FDI_525_INCH;
    trackNum = 0;
    headNum = 0;
    rotationSpeed = 300;
    tracksPerInch = 48;
    headWidth = 48;
}

bool DiskImageFDI::load(DiskImageFile *file)
{
    // Read first page of header
    DIData header;
    header.resize(0x200);
    
    if (!file->read(0, &header.front(), header.size()))
        return false;
    
    // Check signature
    if (strncmp((char *) &header[0], FDI_SIGNATURE, sizeof(FDI_SIGNATURE) - 1))
        return false;
    
    // Check version
    if (getDIShortBE(&header[140]) != 2)
        return false;
    
    // Read header
    trackNum = getDIShortBE(&header[142]) + 1;
    headNum = header[144] + 1;
    rotationSpeed = header[146] + 128;
    readOnly = header[147] & 0x1;
    DIInt tpiCode = header[148];
    
    if (tpiCode < 5)
    {
        DIInt tpiTable[] = {48, 67, 96, 100, 135, 192};
        
        tracksPerInch = tpiTable[tpiCode];
    }
    else
        tracksPerInch = 0;
    
    // Read full header
    header.resize((152 + 2 * trackNum * headNum) / 0x200);
    
    if (!file->read(0, &header.front(), header.size()))
        return false;
    
    return true;
}

void DiskImageFDI::setReadOnly(bool value)
{
    
}

bool DiskImageFDI::isReadOnly()
{
}

void DiskImageFDI::setDiskType(DIFDIDiskType value)
{
}

DIFDIDiskType DiskImageFDI::getDiskType()
{
}

void DiskImageFDI::setHeadNum(DIInt value)
{
}

DIInt DiskImageFDI::getHeadNum()
{

}
void DiskImageFDI::setRotationSpeed(float value)
{
    
}
float DiskImageFDI::getRotationSpeed()
{
    
}
void DiskImageFDI::setTracksPerInch(DIInt value)
{
}

DIInt DiskImageFDI::getTracksPerInch()
{
    
}

void DiskImageFDI::setHeadWidth(DIInt value)
{
    
}

DIInt DiskImageFDI::getHeadWidth()
{

}

bool DiskImageFDI::readTrack(DIInt track, DIInt head, DIData& buf, DIFDIFormat& format)
{
    DIInt index = track * headNum + head;
    
    buf = trackData[index];
    format = trackFormat[index];
    
    return true;
}

bool DiskImageFDI::writeTrack(DIInt track, DIInt head, DIData& trackData, DIFDIFormat format)
{
    return false;
}
