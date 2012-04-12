
/**
 * libdiskimage
 * Block RAW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses RAW disk images
 */

#include "BlockRAW.h"

#define BLOCK_SIZE 512

BlockRAW::BlockRAW()
{    
}

BlockRAW::BlockRAW(string path)
{
    open(path);
}

BlockRAW::BlockRAW(DIData& data)
{
    open(data);
}

bool BlockRAW::open(string path)
{
    if (!diskImage.open(path))
        return false;
    
    return checkSectorOrder();
}

bool BlockRAW::open(DIData& data)
{
    return diskImage.open(data);
}

bool BlockRAW::is_open()
{
    return diskImage.is_open();
}

void BlockRAW::close()
{
    diskImage.close();
    
    appleDOS33Reorder = false;
}

bool BlockRAW::setProperty(string name, string value)
{
    return diskImage.setProperty(name, value);
}

bool BlockRAW::getProperty(string name, string& value)
{
    if (name == "blockNum")
    {
        if (!diskImage.getProperty("imageSize", value))
            return false;
        
        value = getDIString(getDILong(value) / BLOCK_SIZE);
    }
    else
        return diskImage.getProperty(name, value);
    
    return true;
}

bool BlockRAW::readBlock(DIInt blockIndex, DIChar *block)
{
    if (!reorder)
        return diskImage.read(blockIndex * BLOCK_SIZE, block, BLOCK_SIZE);
    
    DIInt sector = 2 * blockIndex;
    if (!diskImage.read(((sector & ~0xf) |
                         BlockRAWSectorReorder[sector & 0xf]) * SECTOR_SIZE,
                        block,
                        SECTOR_SIZE))
        return false;
    
    sector++;
    return diskImage.read(((sector & ~0xf) |
                           BlockRAWSectorReorder[sector & 0xf]) * SECTOR_SIZE,
                          block + SECTOR_SIZE,
                          SECTOR_SIZE);
}

bool BlockRAW::writeBlock(DIInt blockIndex, DIChar *block)
{
    if (!reorder)
        return diskImage.write(blockIndex * BLOCK_SIZE, block, BLOCK_SIZE);
    
    DIInt sector = 2 * blockIndex;
    if (!diskImage.write(((sector & ~0xf) |
                          BlockRAWSectorReorder[sector & 0xf]) * SECTOR_SIZE,
                         block,
                         SECTOR_SIZE))
        return false;
    
    sector++;
    return diskImage.write(((sector & ~0xf) |
                            BlockRAWSectorReorder[sector & 0xf]) * SECTOR_SIZE,
                           block + SECTOR_SIZE,
                           SECTOR_SIZE);
}

bool BlockRAW::checkSectorOrder()
{
    string sectorOrder;
    
    if (!diskImage.getProperty("sectorOrder", sectorOrder))
    {
        close();
        
        return false;
    }
    
/*    string ext = getDIPathExtension(path);
    
    if (ext == "d13")
        sectorOrder = "Apple DOS 3.2";
    else if ((ext == "dsk") || (ext == "do"))
        sectorOrder = "Apple DOS 3.3";
    else if (ext == "cpm")
        sectorOrder = "Apple CP/M";*/
    
    if (sectorOrder == "")
        reorder = false;
    else if (sectorOrder == "Apple DOS 3.3")
        reorder = true;
    else
    {
        close();
        
        return false;
    }
    
    return true;
}
