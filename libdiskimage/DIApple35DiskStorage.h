
/**
 * libdiskimage
 * Apple 3.5" Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple 3.5" disk image
 */

#include "DICommon.h"

class DIApple35
{
public:
    DIApple35();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool isReadOnly();
    
    bool readTrack(DIInt trackIndex, DIInt headIndex, DIData& data);
    bool writeTrack(DIInt trackIndex, DIInt headIndex, DIData& data);
};
