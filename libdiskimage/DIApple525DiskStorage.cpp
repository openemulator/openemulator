
/**
 * libdiskimage
 * Apple 5.25" Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple 5.25" disk image
 */

#include "DIApple525DiskStorage.h"

#define GCR53_TRACKSIZE 256 * 13
#define GCR62_TRACKSIZE 256 * 16
#define NIB_TRACKSIZE   6656
#define NIB2_TRACKSIZE  6384

DIApple525DiskStorage::DIApple525DiskStorage()
{
}

bool DIApple525DiskStorage::open(string path)
{
    return (fileBackingStore.open(path) &&
            open(&fileBackingStore, getDIPathExtension(path)));
}

bool DIApple525DiskStorage::open(DIData& data)
{
    return (ramBackingStore.open(data) &&
            open(&ramBackingStore, ""));
}

void DIApple525DiskStorage::close()
{
    // Cuando cierro
}

bool DIApple525DiskStorage::open(DIBackingStore *backingStore, string pathExtension)
{
    if (twoIMGBackingStore.open(backingStore))
    {
        switch (twoIMGBackingStore.getFormat())
        {
            case DI_2IMG_DOS:
                return encodeGCRDisk(&twoIMGBackingStore, DI_APPLE525_DOS,
                                     twoIMGBackingStore.getGCRVolume());
                
            case DI_2IMG_PRODOS:
                return encodeGCRDisk(&twoIMGBackingStore, DI_APPLE525_PRODOS,
                                     twoIMGBackingStore.getGCRVolume());
                
            case DI_2IMG_NIB:
                return encodeNIBDisk(&twoIMGBackingStore);
                
            default:
                return false;
        }
        
        writeEnabled = twoIMGBackingStore.isWriteEnabled();
    }
    else if (dc42BackingStore.open(backingStore))
        return encodeGCRDisk(&dc42BackingStore, DI_APPLE525_PRODOS, 254);
    else if (fdiDiskStorage.open(backingStore))
        return encodeFDIDisk();
    else if (v2dDiskStorage.open(backingStore))
        return encodeV2DDisk();
    else
    {
        if ((pathExtension == "dsk") ||
            (pathExtension == "do") ||
            (pathExtension == "d13"))
            return encodeGCRDisk(backingStore, DI_APPLE525_DOS, 254);
        else if (pathExtension == "cpm")
            return encodeGCRDisk(backingStore, DI_APPLE525_CPM, 254);
        else
        {
            if (encodeGCRDisk(&dc42BackingStore, DI_APPLE525_PRODOS, 254))
                return true;
            
            return encodeNIBDisk(&dc42BackingStore);
        }
    }
    
    return true;
}

bool DIApple525DiskStorage::isWriteEnabled()
{
    return writeEnabled;
}

DITrack *DIApple525DiskStorage::getTrack(DIInt track, DIInt head)
{
    return NULL;
}

bool DIApple525DiskStorage::encodeGCRDisk(DIBackingStore *backingStore,
                                          DIApple525SectorOrder sectorOrder,
                                          DIInt volume)
{
    DIInt trackSize = 0;
    
    DILong size = backingStore->getSize();
    
    DIInt trackNum;
    for (trackNum = 35; trackNum <= 40; trackNum++)
    {
        if (size == (trackNum * GCR53_TRACKSIZE))
            trackSize = GCR53_TRACKSIZE;
        else if (size == (trackNum * GCR62_TRACKSIZE))
            trackSize = GCR62_TRACKSIZE;
        else
            continue;
        
        break;
    }
    
    if (!trackSize)
        return false;
    
    DIData data;
    data.resize(trackSize);
    
    for (DIInt track = 0; track < trackNum; track++)
    {
        if (!backingStore->read(track * trackSize, &data.front(), trackSize))
            return false;
        
        if (trackSize == GCR53_TRACKSIZE)
            encodeGCR53Track(4 * track, data);
        else
            encodeGCR62Track(4 * track, data, sectorOrder);
    }
    
    return true;
}

bool DIApple525DiskStorage::encodeNIBDisk(DIBackingStore *backingStore)
{
    DILong size = backingStore->getSize();
    DIInt trackSize = 0;
    
    if (size == (35 * NIB_TRACKSIZE))
        trackSize = NIB_TRACKSIZE;
    else if (size == 35 * NIB2_TRACKSIZE)
        trackSize = NIB2_TRACKSIZE;
    else
        return false;
    
    DIData data;
    data.resize(trackSize);
    
    for (DIInt track = 0; track < 35; track++)
    {
        if (!backingStore->read(track * trackSize, &data.front(), trackSize))
            return false;
        
        encodeNIBTrack(4 * track, data);
    }
    
    return true;
}

bool DIApple525DiskStorage::encodeFDIDisk()
{
    DIData data;
    
    for (DIInt track = 0; track < 4 * 40; track++)
    {
        DIFDIFormat format;
        
        fdiDiskStorage.readTrack(0, track, data, format);
        
        DIInt qtrack = track * 192 / fdiDiskStorage.getTracksPerInch();
        
        switch (format)
        {
            case DI_FDI_BLANK:
                break;
                
            case DI_FDI_APPLE_GCR53:
                encodeGCR53Track(qtrack, data);
                
                break;
                
            case DI_FDI_APPLE_GCR62:
                encodeGCR62Track(qtrack, data, DI_APPLE525_PRODOS);
                
                break;
                
            case DI_FDI_RAWGCRFM_250KBPS:
                encodeFDIBitstream(qtrack, data);
                
                break;
                
            case DI_FDI_PULSES:
                encodeFDIPulses(qtrack, data);
                
                break;
                
            default:
                return false;
        }
    }
    
    return true;
}

bool DIApple525DiskStorage::encodeV2DDisk()
{
    DIData data;
    
    for (DIInt qtrack = 0; qtrack < 4 * 40; qtrack++)
    {
        v2dDiskStorage.readTrack(qtrack, data);
        
        if (data.size())
            encodeNIBTrack(qtrack, data);
    }
    
    return true;
}

void DIApple525DiskStorage::encodeGCR53Track(DIInt qtrack, DIData& data)
{
    
}

void DIApple525DiskStorage::encodeGCR62Track(DIInt qtrack, DIData& data,
                                             DIApple525SectorOrder sectorOrder)
{
    
}

void DIApple525DiskStorage::encodeNIBTrack(DIInt qtrack, DIData& data)
{

}

void DIApple525DiskStorage::encodeFDIBitstream(DIInt qtrack, DIData& data)
{
    if (data.size() < 8)
        return;
    
    copy(data.begin() + 8, data.end(), trackData[qtrack].stream.begin());
    trackData[qtrack].bitnum = getDIIntBE(&data[0]);
}

void DIApple525DiskStorage::encodeFDIPulses(DIInt qtrack, DIData& data)
{
    
}
