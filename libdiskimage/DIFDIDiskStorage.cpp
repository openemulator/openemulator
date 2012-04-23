
/**
 * libdiskimage
 * Disk Image FDI
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses FDI disk images
 */

#include "math.h"

#include "DIFDIDiskStorage.h"

#define FDI_SIGNATURE   "Formatted Disk Image File\n\r"
#define FDI_CREATOR     "libdiskimage " DI_VERSION

typedef enum
{
    DI_FDI_BLANK = 0x00,
    DI_FDI_AMIGA_DD,
    DI_FDI_AMIGA_HD,
    DI_FDI_ATARI_DD9,
    DI_FDI_ATARI_DD10,
    DI_FDI_PC_DD8,
    DI_FDI_PC_DD9,
    DI_FDI_PC_HD15,
    DI_FDI_PC_HD16,
    DI_FDI_PC_EHD,
    DI_FDI_C1541,
    DI_FDI_APPLE_DOS32,
    DI_FDI_APPLE_DOS33,
    DI_FDI_APPLE_35,
    DI_FDI_PC_SD,
    DI_FDI_PULSES = 0x80,
    DI_FDI_DECODEDGCRFM_125000BPS = 0xc0,
    DI_FDI_DECODEDGCRFM_150000BPS,
    DI_FDI_DECODEDGCRFM_250000BPS,
    DI_FDI_DECODEDGCRFM_300000BPS,
    DI_FDI_DECODEDGCRFM_500000BPS,
    DI_FDI_DECODEDC1541_ZONE1 = 0xc9,
    DI_FDI_DECODEDC1541_ZONE2,
    DI_FDI_DECODEDC1541_ZONE3,
    DI_FDI_GCRFM_125000BPS = 0xd0,
    DI_FDI_GCRFM_150000BPS,
    DI_FDI_GCRFM_250000BPS,
    DI_FDI_GCRFM_300000BPS,
    DI_FDI_GCRFM_500000BPS,
    DI_FDI_GCRFM_281500BPS,
    DI_FDI_GCRFM_312500BPS,
    DI_FDI_GCRFM_343750BPS,
    DI_FDI_GCRFM_375000BPS,
    DI_FDI_RAWC1541_ZONE1,
    DI_FDI_RAWC1541_ZONE2,
    DI_FDI_RAWC1541_ZONE3,
    DI_FDI_RAWDECODEDMFM_125000BPS = 0xe0,
    DI_FDI_RAWDECODEDMFM_150000BPS,
    DI_FDI_RAWDECODEDMFM_250000BPS,
    DI_FDI_RAWDECODEDMFM_300000BPS,
    DI_FDI_RAWDECODEDMFM_500000BPS,
    DI_FDI_RAWDECODEDMFM_1000000BPS,
    DI_FDI_RAWMFM_125000BPS = 0xf0,
    DI_FDI_RAWMFM_150000BPS,
    DI_FDI_RAWMFM_250000BPS,
    DI_FDI_RAWMFM_300000BPS,
    DI_FDI_RAWMFM_500000BPS,
    DI_FDI_RAWMFM_1000000BPS,
} DIFDITrackFormat;

static const DIInt tpiCode[] =
{
    48, 67, 96, 100, 135, 192
};

DIFDIDiskStorage::DIFDIDiskStorage()
{
    backingStore = NULL;
    
    writeEnabled = false;
    diskType = DI_525_INCH;
    headNum = 0;
    rotationSpeed = 300;
    tracksPerInch = 48;
}

bool DIFDIDiskStorage::openForReading(DIBackingStore *backingStore)
{
    close();
    
    // Read first page of header
    DIData header;
    header.resize(0x200);
    
    if (!backingStore->read(0, &header.front(), header.size()))
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
    writeEnabled = !(header[147] & 0x1);
    tracksPerInch = getTPIFromCode(header[148]);
    
    // Read full header
    DIInt indexNum = trackNum * headNum;
    
    header.resize(((152 + 2 * indexNum) / 0x200) * 0x200);
    
    if (!backingStore->read(0, &header.front(), header.size()))
        return false;
    
    DIInt offset = header.size();
    for (int index = 0; index < indexNum; index++)
    {
        DIInt format = header[152 + 2 * index];
        DIInt size = header[152 + 2 * index + 1];
        
        if ((format & 0xc0) == DI_FDI_PULSES)
        {
            size |= (format & 0x3f) << 8;
            
            format = DI_FDI_PULSES;
        }
        
        trackFormat[index] = (DITrackFormat) format;
        trackOffset[index] = offset;
        trackSize[index] = size * 0x100;
        
        offset += trackSize[index];
    }
    
    this->backingStore = backingStore;
    
    return true;
}

bool DIFDIDiskStorage::openForWriting(DIBackingStore *backingStore, bool writeEnabled,
                                      DIDiskType diskType, DIInt headNum,
                                      float rotationSpeed, DIInt tracksPerInch)
{
    close();
    
    this->writeEnabled = writeEnabled;
    this->diskType = diskType;
    this->headNum = headNum;
    this->rotationSpeed = rotationSpeed;
    this->tracksPerInch = tracksPerInch;
    
    this->backingStore = backingStore;
    
    return true;
}

bool DIFDIDiskStorage::close()
{
    bool writing = false;
    
    if (writing)
    {
        // Fix trackData
        DIInt indexNum = trackData.size();
        indexNum = ceil(indexNum / headNum) * headNum;
        
        trackData.resize(indexNum);
        trackFormat.resize(indexNum);
        
        // Build header
        DIData header;
        header.resize(ceil((152 + 2 * indexNum) / 0x200) * 0x200);
        
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
        setDIShortBE(&header[142], indexNum  / headNum - 1);
        header[144] = headNum - 1;
        header[145] = diskType;
        signed char rotSpeed = rotationSpeed - 128;
        if (rotSpeed < 0)
            rotSpeed = 0;
        else if (rotSpeed > 255)
            rotSpeed = 255;
        header[146] = rotSpeed;
        header[147] = !writeEnabled;
        header[148] = getCodeFromTPI(tracksPerInch);
        header[149] = getCodeFromTPI(tracksPerInch);
        
        for (int i = 0; i < indexNum; i++)
        {
            DIInt format = DI_FDI_GCRFM_250000BPS;
            DIInt size = trackData[i].size() / 0x100;
            
            if (format == DI_FDI_PULSES)
                format |= (size >> 8) & 0x3f;
            
            header[152 + 2 * i] = format;
            header[152 + 2 * i + 1] = size;
        }
        
        // Write header
        if (!backingStore->write(0, &header.front(), header.size()))
            return false;
        
        // Write tracks
        DIInt trackOffset = header.size();
        for (int i = 0; i < indexNum; i++)
        {
            DIInt trackSize = ceil(trackData[i].size() / 0x100) * 0x100;
            trackData[i].resize(trackSize);
            
            if (!backingStore->write(trackOffset, &trackData[i].front(), trackSize))
                return false;
            
            trackOffset += trackSize;
        }
    }
    
    return true;
}

bool DIFDIDiskStorage::isWriteEnabled()
{
    return writeEnabled && backingStore->isWriteEnabled();
}

DIDiskType DIFDIDiskStorage::getDiskType()
{
    return diskType;
}

DIInt DIFDIDiskStorage::getHeadNum()
{
    return headNum;
}

float DIFDIDiskStorage::getRotationSpeed()
{
    return rotationSpeed;
}

DIInt DIFDIDiskStorage::getTracksPerInch()
{
    return tracksPerInch;
}

string DIFDIDiskStorage::getFormatLabel()
{
    string formatLabel = "FDI Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

bool DIFDIDiskStorage::readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    DIInt index = trackIndex * headNum + headIndex;
    DIInt format = trackFormat[index];
    
    switch (format)
    {
        case DI_FDI_APPLE_DOS32:
            track.format = DI_APPLE_DOS32;
            track.bitrate = 250000;
            return backingStore->read(trackOffset[index], &track.data.front(), trackSize[index]);
            
        case DI_FDI_APPLE_DOS33:
            track.format = DI_APPLE_DOS33;
            track.bitrate = 250000;
            return backingStore->read(trackOffset[index], &track.data.front(), trackSize[index]);
            
        case DI_FDI_APPLE_35:
        {
            if (trackIndex >= 80)
                return false;
            
            DIInt gcr35Bitrate[] = { 281250, 312500, 343750, 375000 };
            
            track.format = DI_APPLE_PRODOS;
            track.bitrate = gcr35Bitrate[trackIndex >> 4];
            
            return backingStore->read(trackOffset[index], &track.data.front(), trackSize[index]);
        }
            
        case DI_FDI_GCRFM_125000BPS:
        case DI_FDI_GCRFM_150000BPS:
        case DI_FDI_GCRFM_250000BPS:
        case DI_FDI_GCRFM_300000BPS:
        case DI_FDI_GCRFM_500000BPS:
        case DI_FDI_GCRFM_281500BPS:
        case DI_FDI_GCRFM_312500BPS:
        case DI_FDI_GCRFM_343750BPS:
        case DI_FDI_GCRFM_375000BPS:
        {
            DIData data;
            
            data.resize(trackSize[index]);
            
            if (!backingStore->read(trackOffset[index], &data.front(), trackSize[index]))
                return false;
            
            DIInt formatBitrate[] = {
                125000, 150000, 250000, 300000, 500000, 281500, 312500, 343750, 375000
            };
            
            DIInt bitrate = formatBitrate[format - DI_FDI_GCRFM_125000BPS];
            
            if (!encodeBitstreamTrack(track, data, bitrate))
                return false;
        }
            
        case DI_FDI_PULSES:
            return DI_BITSTREAM;
            
        default:
            return false;
    }
}

DIInt DIFDIDiskStorage::getCodeFromTPI(DIInt value)
{
    for (DIInt i = 0; i < (sizeof(tpiCode) / sizeof(DIInt)); i++)
        if (tpiCode[i] == value)
            return i;
    
    return 0;
}

DIInt DIFDIDiskStorage::getTPIFromCode(DIInt value)
{
    if (value < (sizeof(tpiCode) / sizeof(DIInt)))
        return tpiCode[value];
    
    return 0;
}

bool DIFDIDiskStorage::encodeBitstreamTrack(DITrack& track, DIData& data, DIInt bitrate)
{
    if (data.size() < 8)
        return false;
    
    // Track format is:
    // * 4 bytes little-endian number of bits
    // * 4 bytes little-endian index offset (not used)
    DIInt bitNum = getDIIntLE(&data[0x00]);
    
    track.data.resize(bitNum);
    
    DIInt minSize = (8 + ((bitNum + 7) / 8));
    if (data.size() < minSize)
        return false;
    
    DIChar codeTable[] = {0x00, 0xff};
    for (int i = 0; i < bitNum; i++)
    {
        DIInt byteIndex = i >> 3;
        DIInt bitIndex = 7 - (i & 0x07);
        
        track.data[i] = codeTable[(data[byteIndex] >> bitIndex) & 0x1];
    }
    
    track.format = DI_BITSTREAM;
    track.bitrate = bitrate;
    
    return true;
}

bool DIFDIDiskStorage::decodeBitstreamTrack(DIData& data, DIInt& bitrate, DITrack& track)
{
    DIInt bitNum = track.data.size();
    DIInt byteNum = (bitNum + 7) >> 3;
    
    data.resize(8 + byteNum);
    
    // Write header
    setDIIntLE(&data[0x00], bitNum);
    setDIIntLE(&data[0x04], 0);
    
    for (int i = 0; i < bitNum; i++)
    {
        DIInt byteIndex = i >> 3;
        DIInt bitIndex = 7 - (i & 0x07);
        
        data[byteIndex] |= ((track.data[i] != 0) << bitIndex);
    }
    
    bitrate = track.bitrate;
    
    return true;
}

bool DIFDIDiskStorage::encodePulsesTrack(DITrack& track, DIData& data, DIInt bitrate)
{
    return true;
}

bool DIFDIDiskStorage::decodePulsesTrack(DIData& data, DIInt& bitrate, DITrack& track)
{
    return true;
}
