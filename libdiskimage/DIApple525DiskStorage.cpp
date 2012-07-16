
/**
 * libdiskimage
 * Apple 5.25" Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple 5.25" disk image
 */

#include "DIApple525DiskStorage.h"
#include "AppleIIInterface.h"

#define SECTOR_SIZE             0x100

#define GCR53_SECTORNUM         13
#define GCR53_TRACKSIZE         (SECTOR_SIZE * GCR53_SECTORNUM)
#define GCR62_SECTORNUM         16
#define GCR62_TRACKSIZE         (SECTOR_SIZE * GCR62_SECTORNUM)
#define NIB_TRACKSIZE           6656
#define NIB2_TRACKSIZE          6384

#define MIN_LOGICALTRACKNUM     35
#define MAX_LOGICALTRACKNUM     40

#define MIN_TRACKNUM            (4 * MIN_LOGICALTRACKNUM)
#define MAX_TRACKNUM            (4 * MAX_LOGICALTRACKNUM)

#define DEFAULT_ROTATIONSPEED   300
#define DEFAULT_TRACKSPERINCH   192
#define DEFAULT_BITRATE         (APPLEII_CLOCKFREQUENCY / 4)

#define DEFAULT_TRACKSIZE       (DEFAULT_BITRATE * 60 / DEFAULT_ROTATIONSPEED)

static const DIChar gcr53EncodeMap[] =
{
	0xab, 0xad, 0xae, 0xaf, 0xb5, 0xb6, 0xb7, 0xba, // 0x00
	0xbb, 0xbd, 0xbe, 0xbf, 0xd6, 0xd7, 0xda, 0xdb,
	0xdd, 0xde, 0xdf, 0xea, 0xeb, 0xed, 0xee, 0xef, // 0x10
	0xf5, 0xf6, 0xf7, 0xfa, 0xfb, 0xfd, 0xfe, 0xff,
};

static const DIChar gcr62EncodeMap[] =
{
	0x96, 0x97, 0x9a, 0x9b, 0x9d, 0x9e, 0x9f, 0xa6, // 0x00
	0xa7, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb2, 0xb3,
	0xb4, 0xb5, 0xb6, 0xb7, 0xb9, 0xba, 0xbb, 0xbc, // 0x10
	0xbd, 0xbe, 0xbf, 0xcb, 0xcd, 0xce, 0xcf, 0xd3,
	0xd6, 0xd7, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, // 0x20
	0xdf, 0xe5, 0xe6, 0xe7, 0xe9, 0xea, 0xeb, 0xec,
	0xed, 0xee, 0xef, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, // 0x30
	0xf7, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
};

static const DIInt dos32SectorOrder[] =
{ 
    0, 10, 7, 4, 1, 11, 8, 5, 2, 12, 9, 6, 3
};

static const DIInt dos33SectorOrder[] =
{
    0, 7, 14, 6, 13, 5, 12, 4, 11, 3, 10, 2, 9, 1, 8, 15
};

static const DIInt prodosSectorOrder[] =
{
    0, 8, 1, 9, 2, 10, 3, 11, 4, 12, 5, 13, 6, 14, 7, 15
};

static const DIInt cpmSectorOrder[] = {
    0, 11, 6, 1, 12, 7, 2, 13, 8, 3, 14, 9, 4, 15, 10, 5
};

DIApple525DiskStorage::DIApple525DiskStorage()
{   
    // Build GCR53 decode map
    memset(gcr53DecodeMap, 0xff, sizeof(gcr53DecodeMap));
    
    for (DIInt i = 0; i < sizeof(gcr53EncodeMap); i++)
        gcr53DecodeMap[gcr53EncodeMap[i]] = i;
    
    // Build GCR62 decode map
    memset(gcr62DecodeMap, 0xff, sizeof(gcr62DecodeMap));
    
    for (DIInt i = 0; i < sizeof(gcr62EncodeMap); i++)
        gcr62DecodeMap[gcr62EncodeMap[i]] = i;
    
    forceWriteProtected = false;
    
    close();
}

DIApple525DiskStorage::~DIApple525DiskStorage()
{
    close();
}

bool DIApple525DiskStorage::open(string path)
{
    close();
    
    return fileBackingStore.open(path) && open(&fileBackingStore);
}

bool DIApple525DiskStorage::open(DIData& data)
{
    close();
    
    return ramBackingStore.open(data) && open(&ramBackingStore);
}

bool DIApple525DiskStorage::open(DIBackingStore *backingStore)
{
    DITrackFormat trackFormat;
    DIInt trackSize = 0;
    
    if (twoIMGBackingStore.open(backingStore))
    {
        backingStore = &twoIMGBackingStore;
        
        if (!validateImageSize(backingStore, trackFormat, trackSize))
            return false;
        
        switch (twoIMGBackingStore.getFormat())
        {
            case DI_2IMG_DOS:
                if ((trackFormat != DI_APPLE_DOS32) &&
                    (trackFormat != DI_APPLE_DOS33))
                    return false;
                
                break;
                
            case DI_2IMG_PRODOS:
                if (trackFormat != DI_APPLE_DOS33)
                    return false;
                
                trackFormat = DI_APPLE_PRODOS;
                
                break;
                
            case DI_2IMG_NIB:
                if (trackFormat != DI_APPLE_NIB)
                    return false;
                
                break;
                
            default:
                return false;
        }
        
        gcrVolume = twoIMGBackingStore.getGCRVolume();
    }
    else if (dc42BackingStore.open(backingStore))
    {
        backingStore = &dc42BackingStore;
        
        if (!validateImageSize(backingStore, trackFormat, trackSize))
            return false;
        
        if (trackFormat != DI_APPLE_DOS33)
            return false;
        
        trackFormat = DI_APPLE_PRODOS;
    }
    else if (fdiDiskStorage.open(backingStore, DEFAULT_ROTATIONSPEED))
    {
        if (fdiDiskStorage.getDiskType() != DI_525_INCH)
            return false;
        
        diskStorage = &fdiDiskStorage;
        
        return true;
    }
    else if (v2dDiskStorage.open(backingStore))
    {
        diskStorage = &v2dDiskStorage;
        
        return true;
    }
    else
    {
        if (!validateImageSize(backingStore, trackFormat, trackSize))
            return false;
        
        string pathExtension = getPathExtension(fileBackingStore.getPath());
        
        switch (trackFormat)
        {
            case DI_APPLE_DOS32:
                if ((pathExtension != "d13") &&
                    (pathExtension != "dsk") &&
                    (pathExtension != "do"))
                    return false;
                
                break;
                
            case DI_APPLE_DOS33:
                if (pathExtension == "cpm")
                    trackFormat = DI_APPLE_CPM;
                else if ((pathExtension != "d13") &&
                         (pathExtension != "dsk") &&
                         (pathExtension != "do"))
                    trackFormat = DI_APPLE_PRODOS;
                
                break;
                
            case DI_APPLE_NIB:
                break;
                
            default:
                return false;
        }
    }
    
    if (!logicalDiskStorage.open(backingStore,
                                 DI_525_INCH, 1,
                                 DEFAULT_TRACKSPERINCH / 4, trackFormat, trackSize))
        return false;
    
    diskStorage = &logicalDiskStorage;
    
    return true;
}

bool DIApple525DiskStorage::close()
{
    if (trackDataModified)
    {
        bool save = true;
        
        if ((diskStorage == &logicalDiskStorage) &&
            (logicalDiskStorage.getTrackFormat() != DI_APPLE_NIB))
        {
            DITrackFormat trackFormat = logicalDiskStorage.getTrackFormat();
            
            bool error = false;
            
            for (DIInt i = 0; !error && i < MAX_TRACKNUM; i++)
            {
                if ((i >= trackData.size()) || !trackData[i].size())
                    continue;
                
                if (i % 4)
                    error = true;
                else
                {
                    DITrack track;
                    track.format = trackFormat;
                    
                    if (trackFormat == DI_APPLE_DOS32)
                    {
                        if (!decodeGCR53Track(track, i) && (i < MIN_TRACKNUM))
                            error = true;
                    }
                    else
                    {
                        if (!decodeGCR62Track(track, i) && (i < MIN_TRACKNUM))
                            error = true;
                    }
                }
            }
            
            if (!error)
            {
                // Save
                for (DIInt i = 0; i < MAX_TRACKNUM; i += 4)
                {
                    if (!trackData[i].size())
                        continue;
                    
                    DITrack track;
                    track.format = trackFormat;
                    
                    if (trackFormat == DI_APPLE_DOS32)
                    {
                        if (!decodeGCR53Track(track, i))
                            continue;
                        
                        logicalDiskStorage.writeTrack(0, i / 4, track);
                    }
                    else
                    {
                        if (!decodeGCR62Track(track, i))
                            continue;
                        
                        logicalDiskStorage.writeTrack(0, i / 4, track);
                    }
                }
                
                save = false;
            }
        }
        
        string path = fileBackingStore.getPath();
        
        if (save && (path != ""))
        {
            // Read in all data
            for (DIInt i = 0; i < MAX_TRACKNUM; i++)
            {
                DIData data;
                
                readTrack(i, data);
            }
            
            {
                if (diskStorage == &fdiDiskStorage)
                {
                    fdiDiskStorage.close();
                    fileBackingStore.close();
                }
                else
                    path += ".fdi";
                
//                fdiDiskStorage.close();
//                fileBackingStore.close();
                
                if (fileBackingStore.create(path) &&
                    fdiDiskStorage.create(&fileBackingStore,
                                          true, DI_525_INCH, 1,
                                          DEFAULT_ROTATIONSPEED, DEFAULT_TRACKSPERINCH))
                {
                    for (DIInt i = 0; i < MAX_TRACKNUM; i++)
                    {
                        DITrack track;
                        
                        track.data.swap(trackData[i]);
                        track.format = track.data.size() ? DI_BITSTREAM_250000BPS : DI_BLANK;
                        
                        fdiDiskStorage.writeTrack(0, i, track);
                    }
                }
            }
/*            else
            {
                if (diskStorage == &ddlDiskStorage)
                {
                    ddlDiskStorage.close();
                    fileBackingStore.close();
                }
                else
                    path += ".ddl";
                
                DIFileBackingStore outputBackingStore;
                DIDDLDiskStorage outputDiskStorage;
                
                if (outputBackingStore.create(path) &&
                    outputDiskStorage.create(&outputBackingStore,
                                             DI_525_INCH, DEFAULT_TRACKSPERINCH))
                {
                    for (DIInt i = 0; i < MAX_TRACKNUM; i++)
                    {
                        DITrack track;
                        
                        track.data.swap(trackData[i]);
                        track.format = track.data.size() ? DI_BITSTREAM_250000BPS : DI_BLANK;
                        
                        outputDiskStorage.writeTrack(0, i, track);
                    }
                }
            }*/
        }
    }
    
    logicalDiskStorage.close();
    ddlDiskStorage.close();
    fdiDiskStorage.close();
    v2dDiskStorage.close();
    
    twoIMGBackingStore.close();
    dc42BackingStore.close();
    fileBackingStore.close();
    ramBackingStore.close();
    
    diskStorage = &dummyDiskStorage;
    
    trackData.clear();
    trackDataModified = false;
    
    gcrVolume = 254;
    
    return true;
}

string DIApple525DiskStorage::getPath()
{
    return fileBackingStore.getPath();
}

bool DIApple525DiskStorage::isWriteEnabled()
{
    return !forceWriteProtected && diskStorage->isWriteEnabled();
}

string DIApple525DiskStorage::getFormatLabel()
{
    return diskStorage->getFormatLabel();
}

void DIApple525DiskStorage::setForceWriteProtected(bool value)
{
    forceWriteProtected = value;
}

bool DIApple525DiskStorage::getForceWriteProtected()
{
    return forceWriteProtected;
}

bool DIApple525DiskStorage::readTrack(DIInt trackIndex, DIData& data)
{
    if (trackIndex >= trackData.size())
        trackData.resize(trackIndex + 1);
    
    if (!trackData[trackIndex].size())
    {
        DITrack track;
        
        DIInt tracksPerInch = diskStorage->getTracksPerInch();
        DIInt trackDivisor = (tracksPerInch ?
                              DEFAULT_TRACKSPERINCH / diskStorage->getTracksPerInch() : 1);
        
        if (trackIndex % trackDivisor)
            track.format = DI_BLANK;
        else
        {
            track.format = DI_BITSTREAM_250000BPS;
            
            if (!diskStorage->readTrack(0, trackIndex / trackDivisor, track))
                track.format = DI_BLANK;
        }
        
        switch (track.format)
        {
            case DI_BLANK:
                data.clear();
                data.resize(DEFAULT_TRACKSIZE);
                
                return true;
                
            case DI_APPLE_DOS32:
                if (!encodeGCR53Track(trackIndex, track))
                    return false;
                
                break;
                
            case DI_APPLE_DOS33:
            case DI_APPLE_PRODOS:
            case DI_APPLE_CPM:
                if (!encodeGCR62Track(trackIndex, track))
                    return false;
                
                break;
                
            case DI_APPLE_NIB:
                if (!encodeNIBTrack(trackIndex, track))
                    return false;
                
                break;
                
            case DI_BITSTREAM_250000BPS:
                trackData[trackIndex] = track.data;
                
                break;
                
            default:
                return false;
        }
    }
    
    data = trackData[trackIndex];
    
    return true;
}

bool DIApple525DiskStorage::writeTrack(DIInt trackIndex, DIData& data)
{
    if (trackIndex >= trackData.size())
        trackData.resize(trackIndex + 1);
    
    trackData[trackIndex] = data;
    trackDataModified = true;
    
    return true;
}

bool DIApple525DiskStorage::validateImageSize(DIBackingStore *backingStore,
                                              DITrackFormat& trackFormat, DIInt& trackSize)
{
    DILong size = backingStore->getSize();
    
    for (DIInt t = MIN_LOGICALTRACKNUM; t <= MAX_LOGICALTRACKNUM; t++)
    {
        if (size == (t * GCR53_TRACKSIZE))
        {
            trackFormat = DI_APPLE_DOS32;
            trackSize = GCR53_TRACKSIZE;
            
            return true;
        }
        else if (size == (t * GCR62_TRACKSIZE))
        {
            trackFormat = DI_APPLE_DOS33;
            trackSize = GCR62_TRACKSIZE;
            
            return true;
        }
        else if (size == (t * NIB_TRACKSIZE))
        {
            trackFormat = DI_APPLE_NIB;
            trackSize = NIB_TRACKSIZE;
            
            return true;
        }
        else if (size == (t * NIB2_TRACKSIZE))
        {
            trackFormat = DI_APPLE_NIB;
            trackSize = NIB2_TRACKSIZE;
            
            return true;
        }
    }
    
    return false;
}

const DIInt *DIApple525DiskStorage::getSectorOrder(DITrackFormat trackFormat)
{
    switch (trackFormat)
    {
        case DI_APPLE_DOS33:
            return dos33SectorOrder;
            
        case DI_APPLE_PRODOS:
            return prodosSectorOrder;
            
        case DI_APPLE_CPM:
            return cpmSectorOrder;
            
        default:
            return NULL;
    }
}

bool DIApple525DiskStorage::encodeGCR53Track(DIInt trackIndex, DITrack& track)
{
    if (track.data.size() != GCR53_TRACKSIZE)
        return false;
    
    trackData[trackIndex].resize(DEFAULT_TRACKSIZE);
    
    setStreamData(trackData[trackIndex]);
    
	for (DIInt i = 0; i < GCR53_SECTORNUM; i++)
    {
        writeSync(!i ? 0x80 : 0x28, 36, 32);
		writeGCR53AddressField(trackIndex / 4, dos32SectorOrder[i]);
        
		writeSync(0x0e, 36, 36);
		writeGCR53DataField(&track.data[dos32SectorOrder[i] * SECTOR_SIZE]);
	}
    
    return true;
}

bool DIApple525DiskStorage::encodeGCR62Track(DIInt trackIndex, DITrack& track)
{
    if (track.data.size() != GCR62_TRACKSIZE)
        return false;
    
    const DIInt *sectorOrder = getSectorOrder(track.format);
    
    trackData[trackIndex].resize(DEFAULT_TRACKSIZE);
    
    setStreamData(trackData[trackIndex]);
    
	for (DIInt i = 0; i < GCR62_SECTORNUM; i++)
    {
        writeSync(!i ? 0x80 : 0x14, 40, 32);
		writeGCR62AddressField(trackIndex / 4, i);
        
		writeSync(0x08, 40, 36);
		writeGCR62DataField(&track.data[sectorOrder[i] * SECTOR_SIZE]);
	}
    
    return true;
}

bool DIApple525DiskStorage::encodeNIBTrack(DIInt trackIndex, DITrack& track)
{
    trackData[trackIndex].resize(2 * DEFAULT_TRACKSIZE);
    
    setStreamData(trackData[trackIndex]);
    
	for (DIInt i = 0; i < track.data.size(); i++)
        if (track.data[i] >= 0x80)
            writeNibble(track.data[i], 32);
	
    trackData[trackIndex].resize(getStreamOffset());
    
    if (!trackData[trackIndex].size())
        trackData[trackIndex].resize(DEFAULT_TRACKSIZE);
    
	return true;
}

bool DIApple525DiskStorage::decodeGCR53Track(DITrack &track, DIInt trackIndex)
{
    setStreamData(trackData[trackIndex]);
    
    if (!readNibble())
        return false;
    
    track.data.resize(GCR53_TRACKSIZE);
    
	for (DIInt i = 0; i < GCR53_SECTORNUM; i++)
    {
		DIInt sectorIndex = dos32SectorOrder[i];
        
		if (!readGCR53AddressField(trackIndex / 4, sectorIndex))
			return false;
        
		if (!readGCR53DataField(&track.data[sectorIndex * SECTOR_SIZE]))
			return false;
	}
	
	return true;
}

bool DIApple525DiskStorage::decodeGCR62Track(DITrack &track, DIInt trackIndex)
{
    setStreamData(trackData[trackIndex]);
    
    if (!readNibble())
        return false;
    
    const DIInt *sectorOrder = getSectorOrder(track.format);
    
    track.data.resize(GCR62_TRACKSIZE);
    
	for (DIInt i = 0; i < GCR62_SECTORNUM; i++)
    {
		DIInt sectorIndex = sectorOrder[i];
        
		if (!readGCR62AddressField(trackIndex / 4, i))
			return false;
        
		if (!readGCR62DataField(&track.data[sectorIndex * SECTOR_SIZE]))
			return false;
	}
	
	return true;
}

void DIApple525DiskStorage::writeGCR53AddressField(DIInt trackIndex, DIInt sectorIndex)
{
	writeNibble(0xd5);
	writeNibble(0xaa);
	writeNibble(0xb5);
	
	writeFMValue(gcrVolume);
	writeFMValue(trackIndex);
	writeFMValue(sectorIndex);
	writeFMValue(gcrVolume ^ trackIndex ^ sectorIndex);
	
	writeNibble(0xde);
	writeNibble(0xaa);
	writeNibble(0xeb);
}

void DIApple525DiskStorage::writeGCR62AddressField(DIInt trackIndex, DIInt sectorIndex)
{
	writeNibble(0xd5);
	writeNibble(0xaa);
	writeNibble(0x96);
	
	writeFMValue(gcrVolume);
	writeFMValue(trackIndex);
	writeFMValue(sectorIndex);
	writeFMValue(gcrVolume ^ trackIndex ^ sectorIndex);
	
	writeNibble(0xde);
	writeNibble(0xaa);
	writeNibble(0xeb, 16);
}

void DIApple525DiskStorage::writeGCR53DataField(DIChar *data)
{
	writeNibble(0xd5);
	writeNibble(0xaa);
	writeNibble(0xad);
	
	resetGCR();
	
	writeGCR53Value(data[0xff] & 0x7);
	
    for (DIInt i = 0; i < 0x33; i++)
        writeGCR53Value((((data[i * 5 + 2] >> 0) & 0x7) << 2) |
                        (((data[i * 5 + 3] >> 0) & 0x1) << 1) |
                        (((data[i * 5 + 4] >> 0) & 0x1) << 0));
    for (DIInt i = 0; i < 0x33; i++)
        writeGCR53Value((((data[i * 5 + 1] >> 0) & 0x7) << 2) |
                        (((data[i * 5 + 3] >> 1) & 0x1) << 1) |
                        (((data[i * 5 + 4] >> 1) & 0x1) << 0));
    for (DIInt i = 0; i < 0x33; i++)
        writeGCR53Value((((data[i * 5 + 0] >> 0) & 0x7) << 2) |
                        (((data[i * 5 + 3] >> 2) & 0x1) << 1) |
                        (((data[i * 5 + 4] >> 2) & 0x1) << 0));
	
	for (DIInt block = 0; block < 5; block++)
		for (DISInt i = 0x32; i >= 0; i--)
			writeGCR53Value(data[i * 5 + block] >> 3);
	
	writeGCR53Value(data[0xff] >> 3);
	
	writeGCR53Checksum();
	
	writeNibble(0xde);
	writeNibble(0xaa);
	writeNibble(0xeb, 12);
}

void DIApple525DiskStorage::writeGCR62DataField(DIChar *data)
{
	DIInt swapBit01[] = {0x00, 0x02, 0x01, 0x03};
	DIInt swapBit23[] = {0x00, 0x08, 0x04, 0x0c};
	DIInt swapBit45[] = {0x00, 0x20, 0x10, 0x30};
	
	writeNibble(0xd5);
	writeNibble(0xaa);
	writeNibble(0xad);
	
	resetGCR();
	
	for (DIInt index = 0; index < 0x54; index++)
		writeGCR62Value(swapBit01[data[0x00 + index] & 0x3] |
                        swapBit23[data[0x56 + index] & 0x3] |
					    swapBit45[data[0xac + index] & 0x3]);
	for (DIInt index = 0x54; index < 0x56; index++)
		writeGCR62Value(swapBit01[data[0x00 + index] & 0x3] |
                        swapBit23[data[0x56 + index] & 0x3]);
	
	for (DIInt index = 0; index < SECTOR_SIZE; index++)
		writeGCR62Value(data[index] >> 2);
	
	writeGCR62Checksum();
	
	writeNibble(0xde);
	writeNibble(0xaa);
	writeNibble(0xeb);
}

bool DIApple525DiskStorage::readGCR53AddressField(DIInt trackIndex, DIInt sectorIndex)
{
    DIInt n = 0;
    
    while (n < 2 * (DEFAULT_TRACKSIZE / 8))
    {
        n++;
		if (readNibble() != 0xd5)
			continue;
        n++;
		if (readNibble() != 0xaa)
			continue;
        n++;
		if (readNibble() != 0xb5)
			continue;
		
        n += 8;
		DIChar readVolume = readFMValue();
		DIChar readTrackIndex = readFMValue();
		DIChar readSectorIndex = readFMValue();
		DIChar readChecksum = readFMValue();
        
		if (readChecksum != (readVolume ^ readTrackIndex ^ readSectorIndex))
			continue;
		if (readTrackIndex != trackIndex)
			continue;
		if (readSectorIndex != sectorIndex)
			continue;
		
        n++;
		if (readNibble() != 0xde)
			continue;
        n++;
		if (readNibble() != 0xaa)
			continue;
		
		return true;
	}
	
	return false;
}

bool DIApple525DiskStorage::readGCR62AddressField(DIInt trackIndex, DIInt sectorIndex)
{
    DIInt n = 0;
    
    while (n < 2 * (DEFAULT_TRACKSIZE / 8))
    {
        n++;
		if (readNibble() != 0xd5)
			continue;
        n++;
		if (readNibble() != 0xaa)
			continue;
        n++;
		if (readNibble() != 0x96)
			continue;
		
        n += 8;
		DIChar readVolume = readFMValue();
		DIChar readTrackIndex = readFMValue();
		DIChar readSectorIndex = readFMValue();
		DIChar readChecksum = readFMValue();
        
		if (readChecksum != (readVolume ^ readTrackIndex ^ readSectorIndex))
			continue;
		if (readTrackIndex != trackIndex)
			continue;
		if (readSectorIndex != sectorIndex)
			continue;
		
        n++;
		if (readNibble() != 0xde)
			continue;
        n++;
		if (readNibble() != 0xaa)
			continue;
		
		return true;
	}
	
	return false;
}

bool DIApple525DiskStorage::readGCR53DataField(DIChar *data)
{
	memset(data, 0, SECTOR_SIZE);
	
	for (DIInt j = 0; j < 30; j++)
    {
		if (readNibble() != 0xd5)
			continue;
		if (readNibble() != 0xaa)
			continue;
		if (readNibble() != 0xad)
			continue;
		
		resetGCR();
		
		data[0xff] = readGCR53Value();
		
        for (DIInt index = 0; index < 0x33; index++)
        {
            DIChar value = readGCR53Value();
            
            data[index * 5 + 2] = (value >> 2);
            data[index * 5 + 3] |= (value >> 1) & 0x1;
            data[index * 5 + 4] |= (value >> 0) & 0x1;
        }
        for (DIInt index = 0; index < 0x33; index++)
        {
            DIChar value = readGCR53Value();
            
            data[index * 5 + 1] = value >> 2;
            data[index * 5 + 3] |= (value >> 0) & 0x2;
            data[index * 5 + 4] |= (value << 1) & 0x2;
        }
        for (DIInt index = 0; index < 0x33; index++)
        {
            DIChar value = readGCR53Value();
            
            data[index * 5 + 0] = value >> 2;
            data[index * 5 + 3] |= (value << 1) & 0x4;
            data[index * 5 + 4] |= (value << 2) & 0x4;
        }
		
		for (DIInt block = 0; block < 5; block++)
			for (DISInt i = 0x32; i >= 0; i--)
            {
				data[i * 5 + block] &= 0x7;
				data[i * 5 + block] |= readGCR53Value() << 3;
			}
		
		data[0xff] |= readGCR53Value() << 3;
		
		if (!validateGCR53Checksum())
			return false;
		
		if (readNibble() != 0xde)
			return false;
		if (readNibble() != 0xaa)
			return false;
		
		return true;
	}
	
	return true;
}

bool DIApple525DiskStorage::readGCR62DataField(DIChar *data)
{
	const DIChar swapBit01[] = { 0x00, 0x02, 0x01, 0x03 };
	
	for (DIInt j = 0; j < 12; j++)
    {
		if (readNibble() != 0xd5)
			continue;
		if (readNibble() != 0xaa)
			continue;
		if (readNibble() != 0xad)
			continue;
		
		resetGCR();
		
		for (DIInt i = 0; i < 0x54; i++)
        {
			DIChar value = readGCR62Value();
            
			data[0x00 + i] = swapBit01[(value >> 0) & 0x3];
			data[0x56 + i] = swapBit01[(value >> 2) & 0x3];
            data[0xac + i] = swapBit01[(value >> 4) & 0x3];
		}
		for (DIInt i = 0x54; i < 0x56; i++)
        {
			DIChar value = readGCR62Value();
            
			data[0x00 + i] = swapBit01[(value >> 0) & 0x3];
			data[0x56 + i] = swapBit01[(value >> 2) & 0x3];
		}
		
		for (DIInt i = 0; i < SECTOR_SIZE; i++)
			data[i] |= readGCR62Value() << 2;
		
		if (!validateGCR62Checksum())
			return false;
		
		if (readNibble() != 0xde)
			return false;
		if (readNibble() != 0xaa)
			return false;
		
		return true;
	}
	
	return false;
}

void DIApple525DiskStorage::writeSync(DIInt num, DIInt q3Clocks, DIInt lastQ3Clocks)
{
    num -= 1;
    
    while (num--)
        writeNibble(0xff, q3Clocks);
    
    writeNibble(0xff, lastQ3Clocks);
}

void DIApple525DiskStorage::writeFMValue(DIChar value)
{
	writeNibble((value >> 1) | 0xaa);
	writeNibble(value | 0xaa);
}

DIChar DIApple525DiskStorage::readFMValue()
{
	DIChar value = (readNibble() << 1) | 0x01;
	return readNibble() & value;
}

void DIApple525DiskStorage::resetGCR()
{
	gcrChecksum = 0;
    gcrError = false;
}

bool DIApple525DiskStorage::isGCRError()
{
    return gcrError;
}

void DIApple525DiskStorage::writeGCR53Value(DIChar value)
{
	writeNibble(gcr53EncodeMap[value ^ gcrChecksum]);
    
	gcrChecksum = value;
}

void DIApple525DiskStorage::writeGCR62Value(DIChar value)
{
	writeNibble(gcr62EncodeMap[value ^ gcrChecksum]);
    
	gcrChecksum = value;
}

void DIApple525DiskStorage::writeGCR53Checksum()
{
	writeGCR53Value(0);
}

void DIApple525DiskStorage::writeGCR62Checksum()
{
	writeGCR62Value(0);
}

DIChar DIApple525DiskStorage::readGCR53Value()
{
    DIChar value = gcr53DecodeMap[readNibble()];
    
    if (value == 0xff)
    {
        value = 0;
        gcrError = true;
    }
    
    value ^= gcrChecksum;
    gcrChecksum = value;
    
    return value;
}

DIChar DIApple525DiskStorage::readGCR62Value()
{
    DIChar value = gcr62DecodeMap[readNibble()];
    
    if (value == 0xff)
    {
        value = 0;
        gcrError = true;
    }
    
    value ^= gcrChecksum;
    gcrChecksum = value;
    
    return value;
}

bool DIApple525DiskStorage::validateGCR53Checksum()
{
	return !gcrError && (readGCR53Value() == 0);
}

bool DIApple525DiskStorage::validateGCR62Checksum()
{
	return !gcrError && (readGCR62Value() == 0);
}

void DIApple525DiskStorage::setStreamData(DIData& data)
{
    streamData = &data.front();
    streamSize = (DIInt) data.size();
    streamOffset = 0;
}

DIInt DIApple525DiskStorage::getStreamOffset()
{
    return streamOffset;
}

void DIApple525DiskStorage::writeNibble(DIChar value)
{
    writeNibble(value, 32);
}

void DIApple525DiskStorage::writeNibble(DIChar value, DISInt q3Clocks)
{
    const DIChar codeTable[] = {0x00, 0xff};
    
	while (q3Clocks > 0)
    {
        streamData[streamOffset++] = codeTable[value >> 7];
        streamOffset %= streamSize;
        
        value <<= 1;
        q3Clocks -= 4;
	}
}

DIChar DIApple525DiskStorage::readNibble()
{
    DIChar value = 0;
    
    for (DIInt i = 0; i < streamSize; i++)
    {
        value <<= 1;
        value |= streamData[streamOffset++] & 0x1;
        streamOffset %= streamSize;
        
        if (value & 0x80)
            return value;
    }
    
    return 0;
}
