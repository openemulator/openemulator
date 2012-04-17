
/**
 * libdiskimage
 * Disk Image Apple Block
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple block disk image
 */

#include "DiskImageAppleBlock.h"

#define BLOCK_SIZE 512

DiskImageAppleBlock::DiskImageAppleBlock()
{
    close();
}

bool DiskImageAppleBlock::open(string path)
{
    if (!diskImageFile.open(path))
        return false;
    
    return open();
}

bool DiskImageAppleBlock::open(DIData& data)
{
    if (!diskImageFile.open(data))
        return false;
    
    return open();
}

bool DiskImageAppleBlock::open()
{
    close();
    
    if (diskImage2IMG.open(&diskImageFile))
    {
        // Check size for multiple of BLOCK_SIZE 
        if (diskImage2IMG.getSize() & (BLOCK_SIZE - 1))
            return false;
        
        format = DISKIMAGEAPPLEBLOCK_2IMG;
        
        return true;
    }
    else if (diskImageDC42.open(&diskImageFile))
    {
        // Check size for multiple of BLOCK_SIZE 
        if (diskImage2IMG.getSize() & (BLOCK_SIZE - 1))
            return false;
        
        format = DISKIMAGEAPPLEBLOCK_DC42;
        
        return true;
    }
    else if (diskImageQCOW.open(&diskImageFile))
    {
        format = DISKIMAGEAPPLEBLOCK_QCOW;
        
        return true;
    }
    else if (diskImageVMDK.open(&diskImageFile))
    {
        format = DISKIMAGEAPPLEBLOCK_QCOW;
        
        return true;
    }
    
    // Check that size is multiple of BLOCK_SIZE 
    if (diskImageFile.getSize() & (BLOCK_SIZE - 1))
        return false;
    
    format = DISKIMAGEAPPLEBLOCK_PO;
    
    return true;
}

void DiskImageAppleBlock::close()
{
    format = DISKIMAGEAPPLEBLOCK_PO;
    
    diskImageFile.close();
    diskImage2IMG.close();
    diskImageDC42.close();
    diskImageQCOW.close();
    diskImageVMDK.close();
}

bool DiskImageAppleBlock::isReadOnly()
{
    if (diskImageFile.isReadOnly())
        return true;
    
    if (format == DISKIMAGEAPPLEBLOCK_2IMG)
        return diskImage2IMG.isReadOnly();
    
    return false;
}

DILong DiskImageAppleBlock::getBlockNum()
{
    switch (format)
    {
        case DISKIMAGEAPPLEBLOCK_PO:
        case DISKIMAGEAPPLEBLOCK_DO:
            return diskImageFile.getSize() / BLOCK_SIZE;
            
        case DISKIMAGEAPPLEBLOCK_2IMG:
            return diskImage2IMG.getSize() / BLOCK_SIZE;
            
        case DISKIMAGEAPPLEBLOCK_DC42:
            return diskImageDC42.getSize() / BLOCK_SIZE;
            
        case DISKIMAGEAPPLEBLOCK_QCOW:
            return diskImageQCOW.getBlockNum();
            
        case DISKIMAGEAPPLEBLOCK_VMDK:
            return diskImageVMDK.getBlockNum();
    }
}

string DiskImageAppleBlock::getFormatLabel()
{
    string label;
    
    switch (format)
    {
        case DISKIMAGEAPPLEBLOCK_PO:
            label = "PO Disk Image";
            break;
            
        case DISKIMAGEAPPLEBLOCK_DO:
            label = "DO Disk Image";
            break;
            
        case DISKIMAGEAPPLEBLOCK_2IMG:
            label = "2IMG ";
            break;
            
        case DISKIMAGEAPPLEBLOCK_DC42:
            label = "DiskCopy 4.2 Disk Image";
            break;
            
        case DISKIMAGEAPPLEBLOCK_QCOW:
            label = "QCOW Disk Image";
            break;
            
        case DISKIMAGEAPPLEBLOCK_VMDK:
            label = "VMDK Disk Image";
            break;
    }
    
    if (isReadOnly())
        label += ", read-only";
    
    return label;
}

bool DiskImageAppleBlock::readBlock(DILong index, DIChar *buf)
{
    DILong translateDOtoPO[] = {
        15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0
    };
    
    switch (format)
    {
        case DISKIMAGEAPPLEBLOCK_DO:
            index = (index & ~0xf) | translateDOtoPO[index & 0xf];
            
        case DISKIMAGEAPPLEBLOCK_PO:
            return diskImageFile.read(index * BLOCK_SIZE, buf, BLOCK_SIZE);
            
        case DISKIMAGEAPPLEBLOCK_2IMG:
            return diskImage2IMG.read(index * BLOCK_SIZE, buf, BLOCK_SIZE);
            
        case DISKIMAGEAPPLEBLOCK_DC42:
            return diskImageDC42.read(index * BLOCK_SIZE, buf, BLOCK_SIZE);
            
        case DISKIMAGEAPPLEBLOCK_QCOW:
            return diskImageQCOW.readBlocks(index, buf, 1);
            
        case DISKIMAGEAPPLEBLOCK_VMDK:
            return diskImageVMDK.readBlocks(index, buf, 1);
    }
}

bool DiskImageAppleBlock::writeBlock(DILong index, const DIChar *buf)
{
    switch (format)
    {
        case DISKIMAGEAPPLEBLOCK_DO:
            
        case DISKIMAGEAPPLEBLOCK_PO:
            return diskImageFile.write(index * BLOCK_SIZE, buf, BLOCK_SIZE);
            
        case DISKIMAGEAPPLEBLOCK_2IMG:
            return diskImage2IMG.write(index * BLOCK_SIZE, buf, BLOCK_SIZE);
            
        case DISKIMAGEAPPLEBLOCK_DC42:
            return diskImageDC42.write(index * BLOCK_SIZE, buf, BLOCK_SIZE);
            
        case DISKIMAGEAPPLEBLOCK_QCOW:
            return diskImageQCOW.writeBlocks(index, buf, 1);
            
        case DISKIMAGEAPPLEBLOCK_VMDK:
            return diskImageVMDK.writeBlocks(index, buf, 1);
    }
}
