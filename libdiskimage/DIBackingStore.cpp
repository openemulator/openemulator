
/**
 * libdiskimage
 * Backing Store
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the backing store interface
 */

#include "DIBackingStore.h"

DIBackingStore::DIBackingStore()
{
}

bool DIBackingStore::isWriteEnabled()
{
    return false;
}

DILong DIBackingStore::getSize()
{
    return 0;
}

string DIBackingStore::getFormatLabel()
{
    return "";
}

bool DIBackingStore::read(DILong pos, DIChar *buf, DILong num)
{
    return false;
}

bool DIBackingStore::write(DILong pos, const DIChar *buf, DILong num)
{
    return false;
}
