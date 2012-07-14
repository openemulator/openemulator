
/**
 * libdiskimage
 * Disk Image DDL
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses Disk Description Language storage
 */

#include "DIDDLDiskStorage.h"

DIDDLDiskStorage::DIDDLDiskStorage()
{
    close();
}

bool DIDDLDiskStorage::open(DIBackingStore *backingStore)
{
    close();
    
    this->backingStore = backingStore;
    
    return true;
}

void DIDDLDiskStorage::close()
{
    backingStore = NULL;
}

bool DIDDLDiskStorage::isWriteEnabled()
{
    return backingStore->isWriteEnabled();
}

DIDiskType DIDDLDiskStorage::getDiskType()
{
    return DI_525_INCH;
}

DIInt DIDDLDiskStorage::getTracksPerInch()
{
    return 192;
}

string DIDDLDiskStorage::getFormatLabel()
{
    return "DDL Disk Image";
}

bool DIDDLDiskStorage::readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    return false;
}
