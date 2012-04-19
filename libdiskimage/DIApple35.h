
/**
 * libdiskimage
 * Apple 3.5" Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple 3.5" disk image
 */

#include "DiskImageCommon.h"

class DiskImageApple35
{
public:
    DiskImageApple35();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool isReadOnly();
    
    bool readTrack(DIInt trackIndex, DIInt head, DITrack& data);
    bool writeTrack(DIInt trackIndex, DIInt head, DITrack& data);
};
