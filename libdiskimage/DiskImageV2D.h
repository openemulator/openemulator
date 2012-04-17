
/**
 * libdiskimage
 * Disk Image V2D
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses Virtual II disk images
 */

#include "DiskImageFile.h"

#define DI_V2D_TRACKNUM   (40 * 4)

class DiskImageV2D
{
public:
    DiskImageV2D();
    
    bool open(DiskImageFile *file);
    void close();
    
    bool readTrack(DIInt track, DIData& buf);
    
private:
    DiskImageFile *file;
    
    vector<DIInt> trackOffset;
    vector<DIInt> trackSize;
};
