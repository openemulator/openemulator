
/**
 * libdiskimage
 * Disk Image V2D
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses D5MI storage
 */

#include "DIBackingStore.h"
#include "DIDiskStorage.h"

class DIV2DDiskStorage : public DIDiskStorage
{
public:
    DIV2DDiskStorage();
    
    bool open(DIBackingStore *file);
    void close();
    
    bool isWriteEnabled();
    DIDiskType getDiskType();
    DIInt getHeadNum();
    float getRotationSpeed();
    DIInt getTracksPerInch();
    string getFormatLabel();
    
    bool readTrack(DIInt headIndex, DIInt trackIndex, DITrack& track);
    
private:
    DIBackingStore dummyBackingStore;
    
    DIBackingStore *backingStore;
    
    vector<DIInt> trackOffset;
    vector<DIInt> trackSize;
};
