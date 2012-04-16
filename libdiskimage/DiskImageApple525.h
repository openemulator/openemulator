
/**
 * libdiskimage
 * Apple 5.25" Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple 5.25" disk image
 */

#include "DiskImageCommon.h"

class DiskImageApple525
{
public:
    DiskImageApple525();
    ~DiskImageApple525();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool isReadOnly();
    
    DIDisk *getDisk();
    
private:
    bool open(DiskImageFile *diskImageFile);
};
