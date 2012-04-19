
/**
 * libdiskimage
 * Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the disk image interface
 */

#ifndef _DIDISKIMAGE_H
#define _DIDISKIMAGE_H

#include "DICommon.h"

class DIDiskImage
{
public:
    DIDiskImage();
    
    virtual bool isWriteEnabled();
    virtual DILong getBlockNum();
    virtual string getFormatLabel();
    
    virtual bool readBlocks(DILong index, DIChar *buf, DIInt num);
    virtual bool writeBlocks(DILong index, const DIChar *buf, DIInt num);
};

#endif
