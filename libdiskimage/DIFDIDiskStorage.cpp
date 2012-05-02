
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
    DI_FDI_PC,
    DI_FDI_PC_INDEXMARKS = 0x05,
    DI_FDI_C1541 = 0x0a,
    DI_FDI_APPLE_DOS32,
    DI_FDI_APPLE_DOS33,
    DI_FDI_APPLE_35,
    DI_FDI_PC_FM,
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
    DIShort version = getDIShortBE(&header[140]);
    
    if ((version != 0x0200) ||
        (version != 0x0201))
        return false;
    
    // Read header
    DIInt trackNum = getDIShortBE(&header[142]) + 1;
    headNum = header[144] + 1;
    rotationSpeed = header[146] + 128;
    writeEnabled = !(header[147] & 0x1);
    tracksPerInch = getTPIFromCode(header[148]);
    
    // Read full header (152 header bytes, tracks, and 8 CRC bytes)
    DIInt indexNum = trackNum * headNum;
    
    if (version == 0x0200)
        header.resize(((152 + 2 * indexNum) / 0x200) * 0x200);
    else
        header.resize(((152 + 2 * indexNum + 8) / 0x200) * 0x200);
        
    if (!backingStore->read(0, &header.front(), header.size()))
        return false;
    
    // Analyze tracks
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
    
    // Analyze CRC
    if (version >= 0x201)
    {
        // Check header CRC
        DIInt headerCRC32 = getDIIntBE(&header[header.size() - 0x04]);
        
        if (headerCRC32 != getDICRC32(&header[0x0], header.size() - 0x08))
            return false;
    }
    
    this->backingStore = backingStore;
    
    return true;
}

bool DIFDIDiskStorage::openForWriting(DIBackingStore *backingStore, bool writeEnabled,
                                      DIDiskType diskType, DIInt headNum,
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
        // Fix trackData
        DIInt indexNum = trackData.size();
        indexNum = ceil(indexNum / headNum) * headNum;
        
        trackFormat.resize(indexNum);
        trackData.resize(indexNum);
        
        // Build header (152 header bytes, tracks, and 8 CRC bytes)
        DIData header;
        header.resize(ceil((152 + 2 * indexNum + 8) / 0x200) * 0x200);
        
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
            DIInt format = trackFormat[i];
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
        DIInt offset = header.size();
        for (int i = 0; i < indexNum; i++)
        {
            DIInt size = ceil(trackData[i].size() / 0x100) * 0x100;
            trackData[i].resize(size);
            
            if (!backingStore->write(offset, &trackData[i].front(), size))
                return false;
            
            offset += size;
        }
    }
    
    writing = false;
    
    backingStore = NULL;
    
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
    if (writing)
        return false;
    
    DIInt index = trackIndex * headNum + headIndex;
    DIInt format = trackFormat[index];
    
    DIData data;
    
    data.resize(trackSize[index]);
    
    if (!backingStore->read(trackOffset[index], &data.front(), trackSize[index]))
        return false;
    
    switch (format)
    {
        case DI_FDI_APPLE_DOS32:
            track.format = DI_APPLE_DOS32;
            track.data = data;
            
            return true;
            
        case DI_FDI_APPLE_DOS33:
            track.format = DI_APPLE_DOS33;
            track.data = data;
            
            return true;;
            
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
    
    switch (track.format)
    {
        case DI_BLANK:
            trackFormat[index] = DI_FDI_BLANK;
            trackData[index].clear();
            
            return true;
            
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
    
    decodedData.resize(bitNum);
    
    DIInt minSize = (8 + ((bitNum + 7) / 8));
    if (data.size() < minSize)
        return false;
    
    DIChar codeTable[] = {0x00, 0xff};
    for (int i = 0; i < bitNum; i++)
    {
        DIInt byteIndex = i >> 3;
        DIInt bitIndex = 7 - (i & 0x07);
        
        decodedData[i] = codeTable[(data[byteIndex] >> bitIndex) & 0x1];
    }
    
    return true;
}

bool DIFDIDiskStorage::encodeBitstreamTrack(DIData& encodedData, DIData& data)
{
    DIInt bitNum = data.size();
    DIInt byteNum = (bitNum + 7) >> 3;
    
    encodedData.clear();
    encodedData.resize(8 + byteNum);
    
    // Write header
    setDIIntBE(&encodedData[0x00], bitNum);
    setDIIntBE(&encodedData[0x04], 0);
    
    for (int i = 0; i < bitNum; i++)
    {
        DIInt byteIndex = i >> 3;
        DIInt bitIndex = 7 - (i & 0x07);
        
        encodedData[byteIndex] |= ((data[i] != 0) << bitIndex);
    }
    
    return true;
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
    DIInt minimumStreamOffset = minimumStreamOffset + averageStreamSize;
    DIInt minimumStreamSize = getDIShortBE(&data[0x08]) + (data[0x07] & 0x1f) * 0x10000;
    DIInt minimumStreamCompression = data[0x07] >> 6;
    DIInt maximumStreamOffset = minimumStreamOffset + averageStreamSize;
    DIInt maximumStreamSize = getDIShortBE(&data[0x0b]) + (data[0x0a] & 0x1f) * 0x10000;
    DIInt maximumStreamCompression = data[0x0a] >> 6;
    DIInt indexStreamOffset = minimumStreamOffset + averageStreamSize;
    DIInt indexStreamSize = getDIShortBE(&data[0x0e]) + (data[0x0d] & 0x1f) * 0x10000;
    DIInt indexStreamCompression = data[0x0d] >> 6;
    
    DIFDIStream averageStream, minimumStream, maximumStream, indexStream;
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
    if (!getStream(indexStream,
                   &data[indexStreamOffset],
                   indexStreamSize,
                   indexStreamCompression,
                   pulseNum))
        return false;
    
    DIInt bitNum = 2 * bitRate * 60 / rotationSpeed;
    decodedData.resize(bitNum);
    
    // To-Do
    
    return true;
}

bool DIFDIDiskStorage::getStream(DIFDIStream& stream, DIChar *data, DIInt size, DIInt compression,
                                 DIInt pulseNum)
{
    stream.resize(pulseNum);
    
    if (compression == 0)
    {
        if (size != (sizeof(DIInt) * pulseNum))
            return false;
        
        for (int i = 0; i < pulseNum; i++)
            stream[i] = getDIIntBE(data + i * sizeof(DIInt));
        
        return true;
    }
    else if (compression == 1)
    {
        initHuffman(data, size);
        
        DIInt substreamShift = 1;
        
        while (substreamShift)
        {
            DIInt streamIndex = 0;
            
            // Read substream header
            DIInt header1 = readHuffmanByte();
            DIInt header2 = readHuffmanByte();
            
            substreamShift = ((header2 & 0x7f) << 7) | (header1 & 0x7f);
            huffmanSignExtension = (header1 & 0x80);
            huffman16Bits = (header2 & 0x80);
            
            // Build Huffman tree
            DIFDIHuffmanNode *root = new DIFDIHuffmanNode();
            
            buildHuffmanTree(root);
            
            // Read Huffman tree values
            readHuffmanTreeValues(root);
            
            // Decode Huffman stream
            while (!isHuffmanBitEOF())
            {
                if (streamIndex >= pulseNum)
                {
                    delete root;
                    
                    return false;
                }
                
                DIFDIHuffmanNode *currentNode = root;
                
                while (currentNode->left)
                {
                    if (!readHuffmanBit())
                        currentNode = currentNode->left;
                    else
                        currentNode = currentNode->right;
                }
                
                DIInt value = currentNode->value;
                
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
                
                value <<= substreamShift;
                
                stream[streamIndex++] |= value;
            }
            
            delete root;
        }
    }
    else
        return false;
    
    return true;
}

void DIFDIDiskStorage::initHuffman(DIChar *data, DIInt size)
{
    huffmanStreamData = data;
    huffmanStreamSize = size;
    huffmanStreamBitIndex = 0;
}

DIChar DIFDIDiskStorage::readHuffmanByte()
{
    huffmanStreamBitIndex = 0;
    
    if (huffmanStreamSize)
    {
        huffmanStreamSize--;
        return *(huffmanStreamData++);
    }
    
    return 0;
}

bool DIFDIDiskStorage::readHuffmanBit()
{
    if (huffmanStreamBitIndex >= 8)
    {
        if (!huffmanStreamSize)
            return false;
        
        huffmanStreamValue = readHuffmanByte();
    }
    
    return (huffmanStreamValue << huffmanStreamBitIndex++) & 0x80;
}

bool DIFDIDiskStorage::isHuffmanBitEOF()
{
    return (huffmanStreamSize == 0) && (huffmanStreamBitIndex >= 8);
}

void DIFDIDiskStorage::buildHuffmanTree(DIFDIHuffmanNode *node)
{
    bool hasBranches = readHuffmanBit();
    
    if (hasBranches)
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
        if (huffman16Bits)
            node->value = (readHuffmanByte() << 8) | readHuffmanByte();
        else
            node->value = readHuffmanByte();
	}
    else
    {
        readHuffmanTreeValues(node->left);
        readHuffmanTreeValues(node->right);
    }
}
