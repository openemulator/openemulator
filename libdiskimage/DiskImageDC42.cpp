
/**
 * libdiskimage
 * DiskCopy 4.2 Block Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses DiskCopy 4.2 disk images
 */

#include <string.h>

#include "BlockDC42.h"

#define HEADER_SIZE 0x54

BlockDC42::BlockDC42() : BlockRAW()
{
}

BlockDC42::BlockDC42(string path) : BlockRAW(path)
{
}

BlockDC42::BlockDC42(DIData& data) : BlockRAW(data)
{
}

BlockDC42::~BlockDC42()
{
}

bool BlockDC42::open(string path)
{
    if (!BlockRAW::open(path))
        return false;
    
    if (!openDC42())
    {
        close();
        
        return false;
    }
    
    return true;
}

bool BlockDC42::open(DIData& data)
{
    if (!BlockRAW::open(data))
        return false;
    
    if (!openDC42())
    {
        close();
        
        return false;
    }
    
    return true;
}

void BlockDC42::close()
{
    BlockRAW::close();
    
    imageOffset = 0;
    imageSize = 0;
    
    tagOffset = 0;
    tagSize = 0;
    
    gcrFormat = 0;
}

bool BlockDC42::openDC42()
{
    DIChar header[HEADER_SIZE];
    
    if (!BlockRAW::read(0, header, HEADER_SIZE))
        return false;
    
    // Get image and tag location
    imageOffset = HEADER_SIZE;
    imageSize = getDIIntBE(&header[0x40]);
    
    tagOffset = imageOffset + imageSize;
    tagSize = getDIIntBE(&header[0x44]);
    
    if ((tagOffset + tagSize) != dataSize)
        return false;
    
    // Get GCR Format byte
    gcrFormat = header[0x51];
    
    // Check id
    if (getDIShortBE(&header[0x52]) != 0x0100)
        return false;
    
    return true;
}

bool BlockDC42::getProperty(string name, string& value)
{
    if (name == "imageSize")
        value = imageSize;
    else if (name == "gcrFormat")
        value = getDIString(gcrFormat);
    else
        return BlockRAW::getProperty(name, value);
    
    return true;
}

bool BlockDC42::read(DILong offset, DIChar *data, DILong size)
{
    if ((offset + size) > imageSize)
        return false;
    
    return BlockRAW::read(imageOffset + offset, data, size);
}

bool BlockDC42::write(DILong offset, DIChar *data, DILong size)
{
    if ((offset + size) > imageSize)
        return false;
    
    return BlockRAW::write(imageOffset + offset, data, size);
}

bool BlockDC42::readTag(DILong offset, DIChar *data, DILong size)
{
    if ((offset + size) > tagSize)
        return false;
    
    return BlockRAW::read(tagOffset + offset, data, size);
}

bool BlockDC42::writeTag(DILong offset, DIChar *data, DILong size)
{
    if ((offset + size) > tagSize)
        return false;
    
    return BlockRAW::write(tagOffset + offset, data, size);
}
