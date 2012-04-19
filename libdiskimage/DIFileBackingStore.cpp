
/**
 * libdiskimage
 * File Backing Store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a file backing store
 */

#include "DIFileBackingStore.h"

DIFileBackingStore::DIFileBackingStore()
{
    close();
}

bool DIFileBackingStore::open(string path)
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

void DIFileBackingStore::close()
{
    file.close();
    ifile.close();
    
    dataSize = 0;
}

bool DIFileBackingStore::isWriteEnabled()
{
    return file.is_open();
}

DILong DIFileBackingStore::getSize()
{
    return dataSize;
}

bool DIFileBackingStore::read(DILong pos, DIChar *buf, DILong num)
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
    
    return false;
}

bool DIFileBackingStore::write(DILong pos, const DIChar *buf, DILong num)
{
    if (file.is_open())
    {
        DILong end = pos + num;
        
        if (end >= dataSize)
            dataSize = end;
        
        file.seekg(pos, ios::beg);
        
        file.write((char *) buf, (size_t) num);
        
        return file.good();
    }
    
    return false;
}
