
/**
 * libdiskimage
 * Disk Image FDI
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses FDI storage
 */

#include "DIBackingStore.h"

typedef enum
{
    DI_FDI_8_INCH,
    DI_FDI_525_INCH,
    DI_FDI_35_INCH,
    DI_FDI_3_INCH,
} DIFDIDiskType;

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
    DI_FDI_APPLE_GCR53,
    DI_FDI_APPLE_GCR62,
    DI_FDI_APPLE_GCR35,
    DI_FDI_PC_SD,
    DI_FDI_PULSES = 0x80,
    DI_FDI_RAWDECODEDGCRFM_125KBPS = 0xc0,
    DI_FDI_RAWDECODEDGCRFM_150KBPS,
    DI_FDI_RAWDECODEDGCRFM_250KBPS,
    DI_FDI_RAWDECODEDGCRFM_300KBPS,
    DI_FDI_RAWDECODEDGCRFM_500KBPS,
    DI_FDI_RAWDECODEDC1541_ZONE1 = 0xc9,
    DI_FDI_RAWDECODEDC1541_ZONE2,
    DI_FDI_RAWDECODEDC1541_ZONE3,
    DI_FDI_RAWGCRFM_125KBPS = 0xd0,
    DI_FDI_RAWGCRFM_150KBPS,
    DI_FDI_RAWGCRFM_250KBPS,
    DI_FDI_RAWGCRFM_300KBPS,
    DI_FDI_RAWGCRFM_500KBPS,
    DI_FDI_RAWAPPLEGCR35_ZONE1,
    DI_FDI_RAWAPPLEGCR35_ZONE2,
    DI_FDI_RAWAPPLEGCR35_ZONE3,
    DI_FDI_RAWAPPLEGCR35_ZONE4,
    DI_FDI_RAWC1541_ZONE1,
    DI_FDI_RAWC1541_ZONE2,
    DI_FDI_RAWC1541_ZONE3,
    DI_FDI_RAWDECODEDMFM_125KBPS = 0xe0,
    DI_FDI_RAWDECODEDMFM_150KBPS,
    DI_FDI_RAWDECODEDMFM_250KBPS,
    DI_FDI_RAWDECODEDMFM_300KBPS,
    DI_FDI_RAWDECODEDMFM_500KBPS,
    DI_FDI_RAWDECODEDMFM_1MBPS,
    DI_FDI_RAWMFM_125KBPS = 0xf0,
    DI_FDI_RAWMFM_150KBPS,
    DI_FDI_RAWMFM_250KBPS,
    DI_FDI_RAWMFM_300KBPS,
    DI_FDI_RAWMFM_500KBPS,
    DI_FDI_RAWMFM_1MBPS,
} DIFDIFormat;

class DIFDIDiskStorage
{
public:
    DIFDIDiskStorage();
    
    bool open(DIBackingStore *backingStore);
    void close();
    
    void setWriteEnabled(bool value);
    bool isWriteEnabled();
    void setDiskType(DIFDIDiskType value);
    DIFDIDiskType getDiskType();
    void setHeadNum(DIInt value);
    DIInt getHeadNum();
    void setRotationSpeed(float value);
    float getRotationSpeed();
    void setTracksPerInch(DIInt value);
    DIInt getTracksPerInch();
    void setHeadWidth(DIInt value);
    DIInt getHeadWidth();
    
    bool readTrack(DIInt head, DIInt track, DIData& buf, DIFDIFormat& format);
    bool writeTrack(DIInt head, DIInt track, DIData& buf, DIFDIFormat format);
    
private:
    DIBackingStore *backingStore;
    
    bool writeEnabled;
    DIFDIDiskType diskType;
    DIInt headNum;
    float rotationSpeed;
    DIInt tracksPerInch;
    DIInt headWidth;
    
    vector<DIFDIFormat> trackFormat;
    vector<DIData> trackData;
    
    DIInt getCodeFromTPI(DIInt value);
    DIInt getTPIFromCode(DIInt value);
};
