
/**
 * libdiskimage
 * Disk Image DDL
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses Disk Description Language storage
 */

#include "DIBackingStore.h"
#include "DIDiskStorage.h"

class DIDDLDiskStorage : public DIDiskStorage
{
public:
    DIDDLDiskStorage();
    
    bool open(DIBackingStore *file);
    void close();
    
    bool isWriteEnabled();
    DIDiskType getDiskType();
    DIInt getTracksPerInch();
    string getFormatLabel();
    
    bool readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    
private:
    DIBackingStore *backingStore;
};
