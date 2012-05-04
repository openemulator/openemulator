
/**
 * libdiskimage
 * RAM Backing Store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses a RAM backing store
 */

#include "DIRAMBackingStore.h"

DIRAMBackingStore::DIRAMBackingStore()
{
    close();
}

bool DIRAMBackingStore::open(DIData& data)
{
    close();
    
    this->data.swap(data);
    
    return true;
}

void DIRAMBackingStore::clear()
{
    data.clear();
}

void DIRAMBackingStore::close()
{
    data.resize(0);
}

bool DIRAMBackingStore::isWriteEnabled()
{
    return true;
}

DILong DIRAMBackingStore::getSize()
{
    return data.size();
}

string DIRAMBackingStore::getFormatLabel()
{
    string formatLabel = "Raw Disk Image";
    
    if (!isWriteEnabled())
        formatLabel += " (read-only)";
    
    return formatLabel;
}

bool DIRAMBackingStore::read(DILong pos, DIChar *buf, DIInt num)
{
    if ((pos + num) > data.size())
        return false;
    
    memcpy((char *) buf, &data.front() + pos, (size_t) num);
    
    return true;
}

bool DIRAMBackingStore::write(DILong pos, const DIChar *buf, DIInt num)
{
    if ((pos + num) >= data.size())
        data.resize((size_t) pos + num);
    
    memcpy(&data.front() + pos, buf, (size_t) num);
    
    return true;
}
