
/**
 * libdiskimage
 * Disk Image V2D
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses D5MI storage
 */

#include "DIBackingStore.h"

class DIV2DDiskStorage
{
public:
    DIV2DDiskStorage();
    
    bool open(DIBackingStore *file);
    void close();
    
    bool readTrack(DIInt track, DIData& buf);
    bool writeTrack(DIInt track, DIData& buf);
    
private:
    DIBackingStore *backingStore;
    
    vector<DIData> trackData;
};
