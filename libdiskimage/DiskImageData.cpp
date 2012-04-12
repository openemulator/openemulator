
/**
 * libdiskimage
 * Block RAW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses RAW disk images
 */

#include "DiskImageData.h"

DiskImageData::DiskImageData()
{
    init();
}

DiskImageData::DiskImageData(string path)
{
    init();
}

DiskImageData::DiskImageData(DIData& data)
{
    init();
}

DiskImageData::~DiskImageData()
{
}

void DiskImageData::init()
{
    dataSize = 0;
    
    readOnly = false;
}

bool DiskImageData::open(string path)
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

bool DiskImageData::open(DIData& data)
{
    close();
    
    this->data.swap(data);
    
    dataSize = this->data.size();
    
    return true;
}

bool DiskImageData::is_open()
{
    return (file.is_open() || data.size());
}

void DiskImageData::close()
{
    file.close();
    
    init();
}

bool DiskImageData::setProperty(string name, string value)
{
    if (name == "readOnly")
        readOnly = getDIInt(value);
    else if (name == "sectorOrder")
        sectorOrder = value;
    else
        return false;
    
    return true;
}

bool DiskImageData::getProperty(string name, string& value)
{
    if (name == "readOnly")
        value = getDIString((DIInt) readOnly);
    else if (name == "sectorOrder")
        value = sectorOrder;
    else if (name == "imageSize")
        value = dataSize;
    else
        return false;
    
    return true;
}

bool DiskImageData::read(DILong offset, DIChar *data, DILong size)
{
    DILong end = offset + size;
    
    if (end >= dataSize)
        return false;
    
    if (file.is_open())
    {
        file.seekg(offset, ios::beg);
        
        file.read((char *)data, (size_t) size);
        
        return !(file.failbit || file.badbit);
    }
    else
    {
        memcpy((char *) data, &this->data.front() + offset, (size_t) size);
        
        return true;
    }
}

bool DiskImageData::write(DILong offset, DIChar *data, DILong size)
{
    if (readOnly)
        return false;
    
    DILong end = offset + size;
    
    if (end >= dataSize)
        dataSize = end;
    
    if (file.is_open())
    {
        file.seekg(offset, ios::beg);
        
        file.write((char *)data, (size_t) size);
        
        return !(file.failbit || file.badbit);
    }
    else
    {
        if (dataSize != this->data.size())
            this->data.resize((size_t) dataSize);
        
        memcpy(&this->data.front() + offset, data, (size_t) size);
        
        return true;
    }
}
