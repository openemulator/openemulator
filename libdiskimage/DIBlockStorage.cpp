
/**
 * libdiskimage
 * Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the block storage interface
 */

#include "DIBlockStorage.h"

DIBlockStorage::DIBlockStorage()
{
}

bool DIBlockStorage::isWriteEnabled()
{
    return false;
}

DIInt DIBlockStorage::getBlockNum()
{
    return 0;
}

string DIBlockStorage::getFormatLabel()
{
    return "";
}

bool DIBlockStorage::readBlocks(DIInt index, DIChar *buf, DIInt num)
{
    return false;
}

bool DIBlockStorage::writeBlocks(DIInt index, const DIChar *buf, DIInt num)
{
    return false;
}
