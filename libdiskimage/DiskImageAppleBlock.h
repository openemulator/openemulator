
/**
 * libdiskimage
 * Disk Image Apple Block
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple block disk
 */

#include "DiskImage2IMG.h"
#include "DiskImageDC42.h"
#include "DiskImageQCOW.h"
#include "DiskImageData.h"

typedef enum 
{
    DISKIMAGEAPPLEBLOCK_PO,
    DISKIMAGEAPPLEBLOCK_DO,
    DISKIMAGEAPPLEBLOCK_2IMG,
    DISKIMAGEAPPLEBLOCK_DC42,
    DISKIMAGEAPPLEBLOCK_QCOW,
    DISKIMAGEAPPLEBLOCK_VMDK,
} DiskImageAppleBlockFormat;

class DiskImageAppleBlock
{
public:
    DiskImageAppleBlock();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool isReadOnly();
    DILong getBlockNum();
    string getFormatLabel();
    
    bool readBlock(DILong index, DIChar *buf);
    bool writeBlock(DILong index, const DIChar *buf);
    
private:
    DiskImageAppleBlockFormat format;
    
    DiskImageFile diskImageFile;
    DiskImage2IMG diskImage2IMG;
    DiskImageDC42 diskImageDC42;
    DiskImageQCOW diskImageQCOW;
    DiskImageQCOW diskImageVMDK;
    
    bool open();
};
