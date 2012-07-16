
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

typedef vector<DIData> DIDDLTrack;

class DIDDLDiskStorage : public DIDiskStorage
{
public:
    DIDDLDiskStorage();
    ~DIDDLDiskStorage();
    
    bool open(DIBackingStore *file);
    bool create(DIBackingStore *backingStore, DIDiskType diskType, DIInt tracksPerInch);
    void close();
    
    bool isWriteEnabled();
    DIDiskType getDiskType();
    DIInt getTracksPerInch();
    string getFormatLabel();
    
    bool readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    bool writeTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    
private:
    DIBackingStore *backingStore;
    
    bool writing;
    
    DIDiskType diskType;
    DIInt tracksPerInch;
    
    vector<DIDDLTrack> trackData;
    
    bool writeString(string& s);
};
