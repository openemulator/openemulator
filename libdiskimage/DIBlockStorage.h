
/**
 * libdiskimage
 * Block Storage
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the block storage interface
 */

#ifndef _DIBLOCKSTORAGE_H
#define _DIBLOCKSTORAGE_H

#include "DICommon.h"

class DIBlockStorage
{
public:
    DIBlockStorage();
    
    virtual bool isWriteEnabled();
    virtual DIInt getBlockNum();
    virtual string getFormatLabel();
    
    virtual bool readBlocks(DIInt index, DIChar *buf, DIInt num);
    virtual bool writeBlocks(DIInt index, const DIChar *buf, DIInt num);
};

#endif
