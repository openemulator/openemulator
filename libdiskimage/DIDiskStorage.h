
/**
 * libdiskimage
 * Disk Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the disk storage interface
 */

#ifndef _DIDISKSTORAGE_H
#define _DIDISKSTORAGE_H

#include "DICommon.h"

typedef enum
{
    DI_8_INCH,
    DI_525_INCH,
    DI_35_INCH,
    DI_3_INCH,
} DIDiskType;

typedef enum
{
    DI_BLANK,
    DI_APPLE_DOS32,
    DI_APPLE_DOS33,
    DI_APPLE_CPM,
    DI_APPLE_PRODOS,
    DI_APPLE_NIB,
    DI_BITSTREAM,
} DITrackFormat;

typedef struct
{
    DIData data;
    DITrackFormat format;
    DIInt bitrate;
} DITrack;

class DIDiskStorage
{
public:
    DIDiskStorage();
    
    virtual bool isWriteEnabled();
    virtual DIDiskType getDiskType();
    virtual DIInt getHeadNum();
    virtual float getRotationSpeed();
    virtual DIInt getTracksPerInch();
    virtual string getFormatLabel();
    
    virtual bool readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    virtual bool writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
};

#endif
