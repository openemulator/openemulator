
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

bool DIRAMBackingStore::read(DILong pos, DIChar *buf, DILong num)
{
    DILong end = pos + num;
    
    if (end >= data.size())
        return false;
    
    memcpy((char *) buf, &data.front() + pos, (size_t) num);
    
    return true;
}

bool DIRAMBackingStore::write(DILong pos, const DIChar *buf, DILong num)
{
    DILong end = pos + num;
    
    if (end >= data.size())
        data.resize((size_t) end);
    
    memcpy(&data.front() + pos, buf, (size_t) num);
    
    return true;
}
