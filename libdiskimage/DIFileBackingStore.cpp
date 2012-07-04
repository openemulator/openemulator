
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
    fp = NULL;
    writeEnabled = false;
}

DIFileBackingStore::~DIFileBackingStore()
{
    close();
}

bool DIFileBackingStore::open(string path)
{
    close();
    
    fp = fopen(path.c_str(), "r+b");
    
    if (!fp)
    {
        fp = fopen(path.c_str(), "rb");
        
        if (!fp)
            return false;
    }
    else
        writeEnabled = true;
    
    this->path = path; 
    
    return true;
}

bool DIFileBackingStore::create(string path)
{
    close();
    
    fp = fopen(path.c_str(), "w+b");
    
    if (!fp)
        return false;
    
    writeEnabled = true;
    
    this->path = path; 
    
    return true;
}

void DIFileBackingStore::close()
{
    if (fp)
        fclose(fp);
    
    fp = NULL;
    writeEnabled = false;
    
    path = "";
}

bool DIFileBackingStore::isWriteEnabled()
{
    return writeEnabled;
}

DILong DIFileBackingStore::getSize()
{
    if (!fp)
        return 0;
    
    if (fseek(fp, 0, SEEK_END))
        return 0;
    
    DILong dataSize = ftell(fp);
    
    return dataSize;
}

string DIFileBackingStore::getFormatLabel()
{
    string formatLabel = "Raw Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

bool DIFileBackingStore::read(DILong pos, DIChar *buf, DIInt num)
{
    if (!fp)
        return false;
    
    if (fseek(fp, (long) pos, SEEK_SET))
        return false;
    
    return fread(buf, num, 1, fp);
}

bool DIFileBackingStore::write(DILong pos, const DIChar *buf, DIInt num)
{
    if (!writeEnabled)
        return false;
    
    if (!fp)
        return false;
    
    if (fseek(fp, (long) pos, SEEK_SET))
        return false;
    
    return fwrite(buf, num, 1, fp);
}
