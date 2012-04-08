
/**
 * libdiskimage
 * Block data disk image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a data type for handling disk image data
 */

#include <sstream>

#include "BlockData.h"

BlockData::BlockData()
{
    init();
}

BlockData::BlockData(string path)
{
    init();
}

BlockData::BlockData(DIData& data)
{
    init();
}

BlockData::~BlockData()
{
}

void BlockData::init()
{
    dataSize = 0;
    
    readOnly = false;
}

bool BlockData::open(string path)
{
    close();
    
    file.open(path.c_str());
    
    if (!is_open())
        return false;
    
    file.seekg(0, ios::end);
    dataSize = file.tellg();
    file.seekg(0, ios::beg);
    
    return true;
}

bool BlockData::open(DIData& data)
{
    close();
    
    this->data.swap(data);
    
    dataSize = this->data.size();
    
    return true;
}

bool BlockData::is_open()
{
    return (file.is_open() || data.size());
}

void BlockData::close()
{
    file.close();
    
    init();
}

bool BlockData::setProperty(string name, string value)
{
    if (name == "readOnly")
        readOnly = getDIInt(value);
    else
        return false;
    
    return true;
}

bool BlockData::getProperty(string name, string& value)
{
    if (name == "readOnly")
        value = getDIString((DIInt) readOnly);
    else if (name == "imageSize")
        value = dataSize;
    else
        return false;
    
    return true;
}

bool BlockData::read(DILong offset, DIData& data)
{
    size_t end = (size_t) offset + data.size();
    
    if (end >= dataSize)
        return false;
    
    if (file.is_open())
    {
        file.seekg(offset, ios::beg);
        
        file.read((char *)data.front(), data.size());
        
        return !(file.failbit || file.badbit);
    }
    else
    {
        memcpy(&data.front(), &this->data.front() + offset, data.size());
        
        return true;
    }
}

bool BlockData::write(DILong offset, DIData& data)
{
    if (readOnly)
        return false;
    
    size_t end = (size_t) offset + data.size();
    
    if (end >= dataSize)
        return false;
    
    if (file.is_open())
    {
        file.seekg(offset, ios::beg);
        
        file.write((char *)data.front(), data.size());
        
        return !(file.failbit || file.badbit);
    }
    else
    {
        memcpy(&this->data.front() + offset, &data.front(), data.size());
        
        return true;
    }
}
