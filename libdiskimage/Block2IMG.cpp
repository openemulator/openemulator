
/**
 * libdiskimage
 * 2IMG Block Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Decodes the 2IMG format
 */

#include <string.h>

#include "Block2IMG.h"

Block2IMG::Block2IMG() : BlockRAW()
{
}

Block2IMG::Block2IMG(string path) : BlockRAW(path)
{
}

Block2IMG::Block2IMG(DIData& data) : BlockRAW(data)
{
}

Block2IMG::~Block2IMG()
{
}

bool Block2IMG::open(string path)
{
    if (!BlockData::open(path))
        return false;
    
    if (!open2IMG())
    {
        close();
        
        return false;
    }
    
    return true;
}

bool Block2IMG::open(DIData& data)
{
    if (!BlockData::open(data))
        return false;
    
    if (!open2IMG())
    {
        close();
        
        return false;
    }
    
    return true;
}

void Block2IMG::close()
{
    BlockData::close();
    
    imageOffset = 0;
    imageSize = 0;
    
    gcrVolume = 0;
}

bool Block2IMG::open2IMG()
{
    DIData header;
    
    header.resize(0x40);
    
    if (!BlockData::read(0, header))
        return false;
    
    // Check id
    if (strncmp((char *) &header[0x00], "2IMG", 4))
        return false;
    
    // Check header size
    DIInt headerSize = getDIShortLE(&header[0x08]);
    if (headerSize <= 0x40)
        return false;
    
    // Check 2IMG version
    if (getDIShortLE(&header[0x0a]) != 1)
        return false;
    
    // Check sector order, do not accept NIB order here
    DIInt format = getDIIntLE(&header[0x0c]);
    
    switch (format)
    {
        case 0:
            sectorOrder = BLOCKRAW_APPLEDOS33;
            
            break;
            
        case 1:
            break;
            
        default:
            return false;
    }
    
    // Get flags
    DIInt flags = getDIIntLE(&header[0x10]);
    
    readOnly = flags & 0x80000000;
    if (flags & 0x100)
        gcrVolume = flags & 0xff;
    else
        gcrVolume = 254;
    
    // Get image location
    imageOffset = getDIIntLE(&header[0x18]);
    imageSize = getDIIntLE(&header[0x1c]);
    
    if ((imageOffset + imageSize) > dataSize)
        return false;
    
    return true;
}

bool Block2IMG::getProperty(string name, string& value)
{
    if (name == "gcrVolume")
        value = getDIString(gcrVolume);
    else
        return BlockRAW::getProperty(name, value);
    
    return true;
}

bool Block2IMG::read(DILong offset, DIData& data)
{
    if ((offset + data.size()) > imageSize)
        return false;
    
    return BlockRAW::read(imageOffset + offset, data);
}

bool Block2IMG::write(DILong offset, DIData& data)
{
    if ((offset + data.size()) > imageSize)
        return false;
    
    return BlockRAW::write(imageOffset + offset, data);
}
