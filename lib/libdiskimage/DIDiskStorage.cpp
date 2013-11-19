
/**
 * libdiskimage
 * Disk Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the disk storage interface
 */

#include "DIDiskStorage.h"

DIDiskStorage::DIDiskStorage()
{
}

bool DIDiskStorage::isWriteEnabled()
{
    return false;
}

DIDiskType DIDiskStorage::getDiskType()
{
    return DI_8_INCH;
}

DIInt DIDiskStorage::getTracksPerInch()
{
    return 0;
}

string DIDiskStorage::getFormatLabel()
{
    return "";
}

bool DIDiskStorage::readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    return false;
}

bool DIDiskStorage::writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track)
{
    return false;
}
