
/**
 * libdiskimage
 * Disk Image File
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a disk image file
 */

#include "DiskImageFile.h"

DiskImageFile::DiskImageFile()
{
    dataSize = 0;
}

bool DiskImageFile::open(string path)
{
    close();
    
    file.open(path.c_str());
    
    if (file.is_open())
    {
        file.seekg(0, ios::end);
        dataSize = file.tellg();
        file.seekg(0, ios::beg);
        
        return true;
    }
    
    ifile.open(path.c_str());
    
    if (ifile.is_open())
    {
        ifile.seekg(0, ios::end);
        dataSize = ifile.tellg();
        ifile.seekg(0, ios::beg);
        
        return true;
    }
    
    return false;
}

bool DiskImageFile::open(DIData& data)
{
    close();
    
    this->data.swap(data);
    
    dataSize = this->data.size();
    
    return true;
}

void DiskImageFile::close()
{
    file.close();
    ifile.close();
}

DILong DiskImageFile::getSize()
{
    return dataSize;
}

bool DiskImageFile::isReadOnly()
{
    return ifile.is_open();
}

bool DiskImageFile::read(DILong pos, DIChar *buf, DILong num)
{
    DILong end = pos + num;
    
    if (end >= dataSize)
        return false;
    
    if (file.is_open())
    {
        file.seekg(pos, ios::beg);
        
        file.read((char *) buf, (size_t) num);
        
        return file.good();
    }
    else if (ifile.is_open())
    {
        ifile.seekg(pos, ios::beg);
        
        ifile.read((char *) buf, (size_t) num);
        
        return ifile.good();
    }
    else
    {
        memcpy((char *) buf, &data.front() + pos, (size_t) num);
        
        return true;
    }
}

bool DiskImageFile::write(DILong pos, const DIChar *buf, DILong num)
{
    if (file.is_open())
    {
        file.seekg(pos, ios::beg);
        
        file.write((char *) buf, (size_t) num);
        
        return file.good();
    }
    else if (ifile.is_open())
        return false;
    else
    {
        if (num != this->data.size())
            this->data.resize((size_t) num);
        
        memcpy(&data.front() + pos, buf, (size_t) num);
        
        return true;
    }
}
