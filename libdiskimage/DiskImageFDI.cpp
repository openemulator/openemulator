
/**
 * libdiskimage
 * Disk Image FDI
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses FDI disk images
 */

#include "math.h"

#include "DiskImageFDI.h"

#define FDI_SIGNATURE   "Formatted Disk Image File\n\r"
#define FDI_CREATOR     "libdiskimage " DI_VERSION

DiskImageFDI::DiskImageFDI()
{
    readOnly = false;
    diskType = DI_FDI_525_INCH;
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
    if (memcmp((char *) &header[0], FDI_SIGNATURE, sizeof(FDI_SIGNATURE) - 1))
        return false;
    
    // Check version
    if (getDIShortBE(&header[140]) != 0x0200)
        return false;
    
    // Read header
    DIInt trackNum = getDIShortBE(&header[142]) + 1;
    headNum = header[144] + 1;
    rotationSpeed = header[146] + 128;
    readOnly = header[147] & 0x1;
    tracksPerInch = getTPIFromCode(header[148]);
    headWidth = getTPIFromCode(header[149]);
    
    // Read full header
    DIInt trackDataNum = trackNum * headNum;
    
    header.resize(((152 + 2 * trackDataNum) / 0x200) * 0x200);
    
    if (!file->read(0, &header.front(), header.size()))
        return false;
    
    DIInt trackOffset = header.size();
    for (int i = 0; i < trackDataNum; i++)
    {
        DIInt format = header[152 + 2 * i];
        DIInt size = header[152 + 2 * i + 1];
        
        if ((format & 0xc0) == DI_FDI_PULSES)
        {
            size |= (format & 0x3f) << 8;
            
            format = DI_FDI_PULSES;
        }
        
        DIInt trackSize = size * 0x100;
        
        trackFormat[i] = (DIFDIFormat) format;
        trackData[i].resize(trackSize);
        
        if (!file->read(trackOffset, &trackData[i].front(), trackSize))
            return false;
        
        trackOffset += size;
    }
    
    return true;
}

bool DiskImageFDI::save(DiskImageFile *file)
{
    // Fix trackDataNum
    DIInt trackDataNum = trackData.size();
    trackDataNum = ceil(trackDataNum / headNum) * headNum;
    
    trackData.resize(trackDataNum);
    trackFormat.resize(trackDataNum);
    
    // Build header
    DIData header;
    header.resize(ceil((152 + 2 * trackDataNum) / 0x200) * 0x200);
    
    // Set signature
    memcpy((char *) &header[0], FDI_SIGNATURE, sizeof(FDI_SIGNATURE) - 1);
    
    // Set creator
    memset(&header[0], ' ', 30);
    memcpy(&header[0], FDI_CREATOR, sizeof(FDI_CREATOR) - 1);
    header[57] = '\n';
    header[58] = '\r';
    
    // Set comment
    memset(&header[59], 0x1a, 80);
    header[139] = 0x1a;
    
    // Set header data
    setDIShortBE(&header[140], 0x0200);
    setDIShortBE(&header[142], trackDataNum  / headNum - 1);
    header[144] = headNum - 1;
    header[145] = diskType;
    signed char rotSpeed = rotationSpeed - 128;
    if (rotSpeed < 0)
        rotSpeed = 0;
    else if (rotSpeed > 255)
        rotSpeed = 255;
    header[146] = rotSpeed;
    header[147] = readOnly;
    header[148] = getCodeFromTPI(tracksPerInch);
    header[149] = getCodeFromTPI(headWidth);
    
    for (int i = 0; i < trackDataNum; i++)
    {
        DIInt format = trackFormat[i];
        DIInt size = trackData[i].size() / 0x100;
        
        if (format == DI_FDI_PULSES)
            format |= (size >> 8) & 0x3f;
        
        header[152 + 2 * i] = format;
        header[152 + 2 * i + 1] = size;
    }
    
    // Write header
    if (!file->write(0, &header.front(), header.size()))
        return false;
    
    // Write tracks
    DIInt trackOffset = header.size();
    for (int i = 0; i < trackDataNum; i++)
    {
        DIInt trackSize = ceil(trackData[i].size() / 0x100) * 0x100;
        trackData[i].resize(trackSize);
        
        if (!file->write(trackOffset, &trackData[i].front(), trackSize))
            return false;
        
        trackOffset += trackSize;
    }
    
    return true;
}

void DiskImageFDI::setReadOnly(bool value)
{
    readOnly = value;
}

bool DiskImageFDI::isReadOnly()
{
    return readOnly;
}

void DiskImageFDI::setDiskType(DIFDIDiskType value)
{
    diskType = value;
}

DIFDIDiskType DiskImageFDI::getDiskType()
{
    return diskType;
}

void DiskImageFDI::setHeadNum(DIInt value)
{
    headNum = value;
}

DIInt DiskImageFDI::getHeadNum()
{
    return headNum;
}

void DiskImageFDI::setRotationSpeed(float value)
{
    rotationSpeed = value;
}

float DiskImageFDI::getRotationSpeed()
{
    return rotationSpeed;
}

void DiskImageFDI::setTracksPerInch(DIInt value)
{
    tracksPerInch = value;
}

DIInt DiskImageFDI::getTracksPerInch()
{
    return tracksPerInch;
}

void DiskImageFDI::setHeadWidth(DIInt value)
{
    headWidth = value;
}

DIInt DiskImageFDI::getHeadWidth()
{
    return headWidth;
}

bool DiskImageFDI::readTrack(DIInt track, DIInt head, DIData& buf, DIFDIFormat& format)
{
    DIInt index = track * headNum + head;
    
    buf = trackData[index];
    format = trackFormat[index];
    
    return true;
}

bool DiskImageFDI::writeTrack(DIInt track, DIInt head, DIData& buf, DIFDIFormat format)
{
    DIInt index = track * headNum + head;
    
    trackData[index] = buf;
    trackFormat[index] = format;
    
    return true;
}

DIInt DiskImageFDI::getCodeFromTPI(DIInt value)
{
    switch (value)
    {
        case 48:
            return 0;
        case 67:
            return 1;
        case 96:
            return 2;
        case 100:
            return 3;
        case 135:
            return 4;
        case 192:
            return 5;
        default:
            return 0;
    }
}

DIInt DiskImageFDI::getTPIFromCode(DIInt value)
{
    switch (value)
    {
        case 0:
            return 48;
        case 1:
            return 67;
        case 2:
            return 96;
        case 3:
            return 100;
        case 4:
            return 135;
        case 5:
            return 192;
        default:
            return 0;
    }
}