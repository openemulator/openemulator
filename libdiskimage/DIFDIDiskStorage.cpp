
/**
 * libdiskimage
 * Disk Image FDI
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses FDI disk images
 */

#include "DIFDIDiskStorage.h"

#define FDI_SIGNATURE   "Formatted Disk Image file"
#define FDI_CREATOR     "libdiskimage " DI_VERSION

typedef enum
{
    DI_FDI_BLANK = 0x00,
    DI_FDI_AMIGA_DD,
    DI_FDI_AMIGA_HD,
    DI_FDI_LOGICAL_03,
    DI_FDI_LOGICAL_04,
    DI_FDI_LOGICAL_05,
    DI_FDI_LOGICAL_06,
    DI_FDI_LOGICAL_07,
    DI_FDI_LOGICAL_08,
    DI_FDI_LOGICAL_09,
    DI_FDI_C1541,
    DI_FDI_APPLE_DOS32,
    DI_FDI_APPLE_DOS33,
    DI_FDI_APPLE_35,
    DI_FDI_LOGICAL_0E,
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
    writing = false;
    
    close();
}

DIFDIDiskStorage::~DIFDIDiskStorage()
{
    close();
}

bool DIFDIDiskStorage::open(DIBackingStore *backingStore, float rotationSpeed)
{
    close();
    
    // Read first page of header
    DIData header;
    header.resize(0x200);
    
    if (!backingStore->read(0, &header.front(), (DIInt) header.size()))
        return false;
    
    // Check signature
    if (memcmp((char *) &header[0], FDI_SIGNATURE, sizeof(FDI_SIGNATURE) - 1))
        return false;
    
    // Check version
    DIShort version = getDIShortBE(&header[140]);
    
    if ((version != 0x0100) &&
        (version != 0x0200) &&
        (version != 0x0201))
        return false;
    
    // Read header
    DIInt trackNum = getDIShortBE(&header[142]) + 1;
    headNum = header[144] + 1;
    writeEnabled = !(header[147] & 0x1);
    
    if (version != 0x0100)
        tracksPerInch = getTPIFromCode(header[148]);
    else
        tracksPerInch = 48;
    
    // Read full header (152 header bytes, tracks, and 8 CRC bytes)
    DIInt indexNum = trackNum * headNum;
    
    if (version == 0x0201)
        header.resize(((0x1ff + 152 + 2 * indexNum + 8) / 0x200) * 0x200);
    else
        header.resize(((0x1ff + 152 + 2 * indexNum) / 0x200) * 0x200);
    
    if (!backingStore->read(0, &header.front(), (DIInt) header.size()))
        return false;
    
    // Analyze tracks
    trackFormat.resize(indexNum);
    trackOffset.resize(indexNum);
    trackSize.resize(indexNum);
    
    DIInt offset = (DIInt) header.size();
    for (DIInt index = 0; index < indexNum; index++)
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
    
    this->rotationSpeed = rotationSpeed;
    
    this->backingStore = backingStore;
    
    return true;
}

bool DIFDIDiskStorage::create(DIBackingStore *backingStore,
                              bool writeEnabled, DIDiskType diskType, DIInt headNum,
                              float rotationSpeed, DIInt tracksPerInch)
{
    close();
    
    writing = true;
    
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
    if (writing)
    {
        // Fix index number
        DIInt indexNum = (DIInt) trackData.size();
        indexNum = ((indexNum + headNum - 1) / headNum) * headNum;
        
        trackFormat.resize(indexNum);
        trackData.resize(indexNum);
        
        // Build header (152 header bytes, tracks)
        DIData header;
        header.resize((((0x200 - 1) + 152 + 2 * indexNum) / 0x200) * 0x200);
        
        // Set signature
        memcpy((char *) &header[0], FDI_SIGNATURE, sizeof(FDI_SIGNATURE) - 1);
        
        // Set creator
        memset(&header[27], ' ', 30);
        memcpy(&header[27], FDI_CREATOR, sizeof(FDI_CREATOR) - 1);
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
        DISInt rotSpeed = rotationSpeed - 128;
        if (rotSpeed < 0)
            rotSpeed = 0;
        else if (rotSpeed > 255)
            rotSpeed = 255;
        header[146] = rotSpeed;
        header[147] = !writeEnabled;
        header[148] = getCodeFromTPI(tracksPerInch);
        header[149] = getCodeFromTPI(tracksPerInch);
        
        for (DIInt i = 0; i < indexNum; i++)
        {
            DIInt format = trackFormat[i];
            DIInt size = (DIInt) (trackData[i].size() + 0xff) / 0x100;
            
            if (format == DI_FDI_PULSES)
                format |= (size >> 8) & 0x3f;
            
            header[152 + 2 * i] = format;
            header[152 + 2 * i + 1] = size;
        }
        
        // Write header
        if (!backingStore->write(0, &header.front(), (DIInt) header.size()))
            return false;
        
        // Write tracks
        DIInt offset = (DIInt) header.size();
        for (DIInt i = 0; i < indexNum; i++)
        {
            DIInt size = ((0xff + (DIInt) trackData[i].size()) / 0x100) * 0x100;
            trackData[i].resize(size);
            
            if (!backingStore->write(offset, &trackData[i].front(), size))
                return false;
            
            offset += size;
        }
    }
    
    backingStore = NULL;
    
    writing = false;
    
    writeEnabled = false;
    diskType = DI_525_INCH;
    headNum = 0;
    rotationSpeed = 0;
    tracksPerInch = 0;
    
    trackFormat.clear();
    trackOffset.clear();
    trackSize.clear();
    
    trackData.clear();
    
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
    if (writing)
        return false;
    
    DIInt index = trackIndex * headNum + headIndex;
    if (index >= trackFormat.size())
        return false;
    
    DIInt format = trackFormat[index];
    
    DIData data;
    
    if (trackSize[index])
    {
        data.resize(trackSize[index]);
        
        if (!backingStore->read(trackOffset[index], &data.front(), trackSize[index]))
            return false;
    }
    
    switch (format)
    {
        case DI_FDI_APPLE_DOS32:
            track.format = DI_APPLE_DOS32;
            track.data = data;
            
            return true;
            
        case DI_FDI_APPLE_DOS33:
            track.format = DI_APPLE_DOS33;
            track.data = data;
            
            return true;
            
        case DI_FDI_APPLE_35:
            track.format = DI_APPLE_PRODOS;
            track.data = data;
            
            return true;
            
        case DI_FDI_GCRFM_250000BPS:
            track.format = DI_BITSTREAM_250000BPS;
            
            return decodeBitstreamTrack(track.data, data);
            
        case DI_FDI_GCRFM_500000BPS:
            track.format = DI_BITSTREAM_500000BPS;
            
            return decodeBitstreamTrack(track.data, data);
            
        case DI_FDI_PULSES:
            if (track.format == DI_BITSTREAM_250000BPS)
                return decodePulsesTrack(track.data, data, 250000);
            else if (track.format == DI_BITSTREAM_500000BPS)
                return decodePulsesTrack(track.data, data, 500000);
            else
                return false;
            
        default:
            return false;
    }
}

bool DIFDIDiskStorage::writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    if (!writing)
        return false;
    
    DIInt index = trackIndex * headNum + headIndex;
    if (index >= trackFormat.size())
    {
        trackFormat.resize(index + 1);
        trackData.resize(index + 1);
    }
    
    switch (track.format)
    {
        case DI_APPLE_DOS32:
            trackFormat[index] = DI_FDI_APPLE_DOS32;
            trackData[index] = track.data;
            
            return true;
            
        case DI_APPLE_DOS33:
            trackFormat[index] = DI_FDI_APPLE_DOS33;
            trackData[index] = track.data;
            
            return true;
            
        case DI_APPLE_PRODOS:
            trackFormat[index] = DI_FDI_APPLE_35;
            trackData[index] = track.data;
            
            return true;
            
        case DI_BITSTREAM_250000BPS:
            trackFormat[index] = DI_FDI_GCRFM_250000BPS;
            
            if (!encodeBitstreamTrack(trackData[index], track.data))
                return false;
            
            return true;
            
        case DI_BITSTREAM_500000BPS:
            trackFormat[index] = DI_FDI_GCRFM_250000BPS;
            
            if (!encodeBitstreamTrack(trackData[index], track.data))
                return false;
            
            return true;
            
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

bool DIFDIDiskStorage::decodeBitstreamTrack(DIData& decodedData, DIData& data)
{
    if (data.size() < 8)
        return false;
    
    // Track format is:
    // * 4 bytes big-endian number of bits
    // * 4 bytes big-endian index offset (not used)
    DIInt bitNum = getDIIntBE(&data[0x00]);
    
    if ((bitNum == 0) || (bitNum >= (1 << 24)))
        return false;
    
    decodedData.resize(bitNum);
    
    DIInt minSize = (8 + ((bitNum + 7) / 8));
    if (data.size() < minSize)
        return false;
    
    DIChar codeTable[] = {0x00, 0xff};
    for (DIInt i = 0; i < bitNum; i++)
    {
        DIInt byteIndex = i >> 3;
        DIInt bitIndex = 7 - (i & 0x07);
        
        decodedData[i] = codeTable[(data[8 + byteIndex] >> bitIndex) & 0x1];
    }
    
    return true;
}

bool DIFDIDiskStorage::encodeBitstreamTrack(DIData& encodedData, DIData& data)
{
    DIInt bitNum = (DIInt) data.size();
    DIInt byteNum = (bitNum + 7) >> 3;
    
    encodedData.clear();
    encodedData.resize(8 + byteNum);
    
    // Write header
    setDIIntBE(&encodedData[0x00], bitNum);
    setDIIntBE(&encodedData[0x04], 0);
    
    for (DIInt i = 0; i < bitNum; i++)
    {
        DIInt byteIndex = i >> 3;
        DIInt bitIndex = 7 - (i & 0x07);
        
        encodedData[8 + byteIndex] |= ((data[i] != 0) << bitIndex);
    }
    
    return true;
}

DIInt DIFDIDiskStorage::getIndexHoleCount(DIInt value)
{
    DIInt zeroCount = (value & 0x00ff) >> 0;
    DIInt oneCount = (value & 0xff00) >> 8;
    
    return zeroCount + oneCount;
}

bool DIFDIDiskStorage::decodePulsesTrack(DIData& decodedData, DIData& data, DIInt bitRate)
{
    if (data.size() < 0x10)
        return false;
    
    // Track format is:
    // * 4 bytes little-endian number of pulses
    // * 3 bytes little-endian size in bytes of average stream
    // * 3 bytes little-endian size in bytes of minimum stream
    // * 3 bytes little-endian size in bytes of maximum stream
    // * 3 bytes little-endian size in bytes of index stream
    DIInt pulseNum = getDIIntBE(&data[0x00]);
    DIInt averageStreamOffset = 0x10;
    DIInt averageStreamSize = getDIShortBE(&data[0x05]) + (data[0x04] & 0x1f) * 0x10000;
    DIInt averageStreamCompression = data[0x04] >> 6;
    DIInt minimumStreamOffset = averageStreamOffset + averageStreamSize;
    DIInt minimumStreamSize = getDIShortBE(&data[0x08]) + (data[0x07] & 0x1f) * 0x10000;
    DIInt minimumStreamCompression = data[0x07] >> 6;
    DIInt maximumStreamOffset = minimumStreamOffset + minimumStreamSize;
    DIInt maximumStreamSize = getDIShortBE(&data[0x0b]) + (data[0x0a] & 0x1f) * 0x10000;
    DIInt maximumStreamCompression = data[0x0a] >> 6;
    DIInt indexHoleStreamOffset = maximumStreamOffset + maximumStreamSize;
    DIInt indexHoleStreamSize = getDIShortBE(&data[0x0e]) + (data[0x0d] & 0x1f) * 0x10000;
    DIInt indexHoleStreamCompression = data[0x0d] >> 6;
    
    DIFDIStream averageStream, minimumStream, maximumStream, indexHoleStream;
    if (!getStream(averageStream,
                   &data[averageStreamOffset],
                   averageStreamSize,
                   averageStreamCompression,
                   pulseNum))
        return false;
    if (!getStream(minimumStream,
                   &data[minimumStreamOffset],
                   minimumStreamSize,
                   minimumStreamCompression,
                   pulseNum))
        return false;
    if (!getStream(maximumStream,
                   &data[maximumStreamOffset],
                   maximumStreamSize,
                   maximumStreamCompression,
                   pulseNum))
        return false;
    if (!getStream(indexHoleStream,
                   &data[indexHoleStreamOffset],
                   indexHoleStreamSize,
                   indexHoleStreamCompression,
                   pulseNum))
        return false;
    
    float averageBitNum = 60.0 * bitRate / rotationSpeed;
    decodedData.resize(2 * averageBitNum);
    
    // Calculate total pulses time
    DIInt totalPulseTime = 0;
    DIInt maxIndexHoleCount = 1;
    for (DIInt i = 0; i < pulseNum; i++)
    {
        totalPulseTime += averageStream[i];
        
        DIInt indexHoleCount = getIndexHoleCount(indexHoleStream[i]);
        if (maxIndexHoleCount < indexHoleCount)
            maxIndexHoleCount = indexHoleCount;
    }
    
    // Sample pulses at constant bit rate
    DIInt averagePulseTime = totalPulseTime / averageBitNum;
    DIInt halfAveragePulseTime = averagePulseTime / 2;
    DIInt index = 0;
    for (DIInt i = 0; i < pulseNum; i++)
    {
        DIInt deltaIndex = (averageStream[i] + halfAveragePulseTime) / averagePulseTime;
        DIInt indexHoleCount = getIndexHoleCount(indexHoleStream[i]);
        
        if (deltaIndex <= 0)
            continue;
        
        index += deltaIndex;
        
        if (index >= decodedData.size())
            return false;
        
        decodedData[index - 1] = 0xff * indexHoleCount / maxIndexHoleCount;
    }
    
    decodedData.resize(index);
    
    return true;
}

bool DIFDIDiskStorage::getStream(DIFDIStream& stream, DIChar *data, DIInt size, DIInt compression,
                                 DIInt pulseNum)
{
    stream.clear();
    stream.resize(pulseNum);
    
    switch (compression)
    {
        case 0:
        {
            if (size != (pulseNum * sizeof(DIInt)))
                return false;
            
            for (DIInt i = 0; i < pulseNum; i++)
                stream[i] = getDIIntBE(data + i * sizeof(DIInt));
            
            return true;
        }
            
        case 1:
        {
            initStream(data, size);
            
            DIInt substreamShift;
            
            do
            {
                DIInt streamIndex = 0;
                
                // Read substream header
                DIInt header1 = readStreamByte();
                DIInt header2 = readStreamByte();
                
                substreamShift = (header1 & 0x7f);
                huffmanSignExtension = (header1 & 0x80);
                huffman16Bits = (header2 & 0x80);
                
                // Build Huffman tree
                DIFDIHuffmanNode *root = new DIFDIHuffmanNode();
                
                buildHuffmanTree(root);
                
                // Read Huffman tree values
                readHuffmanTreeValues(root);
                
                // Decode Huffman stream
                for (DIInt i = 0; i < pulseNum; i++)
                {
                    DIFDIHuffmanNode *currentNode = root;
                    
                    while (currentNode->left)
                    {
                        if (isStreamEnd())
                        {
                            delete root;
                            
                            return false;
                        }
                        
                        if (!readStreamBit())
                            currentNode = currentNode->left;
                        else
                            currentNode = currentNode->right;
                    }
                    
                    stream[streamIndex++] |= currentNode->value << substreamShift;
                }
                
                delete root;
            } while (substreamShift);
            
            return true;
        }
            
        default:
            return false;
    }
}

void DIFDIDiskStorage::initStream(DIChar *data, DIInt size)
{
    streamData = data;
    streamSize = size;
    streamBitMask = 0;
}

DIChar DIFDIDiskStorage::readStreamByte()
{
    streamBitMask = 0;
    
    if (!streamSize)
        return 0;
    
    streamSize--;
    return *(streamData++);
}

bool DIFDIDiskStorage::readStreamBit()
{
    if (!streamBitMask)
    {
        streamValue = readStreamByte();
        streamBitMask = 0x80;
    }
    
    bool bit = (streamValue & streamBitMask);
    streamBitMask >>= 1;
    
    return bit;
}

bool DIFDIDiskStorage::isStreamEnd()
{
    return !streamSize && !streamBitMask;
}

void DIFDIDiskStorage::buildHuffmanTree(DIFDIHuffmanNode *node)
{
    if (isStreamEnd())
        return;
    
    bool isLeaf = readStreamBit();
    
    if (!isLeaf)
    {
		node->left = new DIFDIHuffmanNode();
        buildHuffmanTree(node->left);
        
		node->right = new DIFDIHuffmanNode();
		buildHuffmanTree(node->right);
    }
}

void DIFDIDiskStorage::readHuffmanTreeValues(DIFDIHuffmanNode *node)
{
	if (!node->left)
    {
        DIInt value;
        
        if (huffman16Bits)
            value = (readStreamByte() << 8) | readStreamByte();
        else
            value = readStreamByte();
        
        if (huffmanSignExtension)
        {
            if (huffman16Bits)
            {
                if (value & 0x8000)
                    value |= 0xffff0000;
            }
            else
            {
                if (value & 0x80)
                    value |= 0xffffff00;
            }
        }
        
        node->value = value;
	}
    else
    {
        readHuffmanTreeValues(node->left);
        readHuffmanTreeValues(node->right);
    }
}
