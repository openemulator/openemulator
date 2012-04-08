
/**
 * libdiskimage
 * Block RAW disk image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a data type for handling raw disk images
 */

#include <fstream>

#include "BlockData.h"

typedef enum
{
    BLOCKRAW_DEFAULT,
    BLOCKRAW_APPLEDOS33,
    BLOCKRAW_APPLECPM,
} BlockRAWSectorOrder;

class BlockRAW : public BlockData
{
public:
    BlockRAW();
    BlockRAW(string path);
    BlockRAW(DIData& data);
    ~BlockRAW();
    
    bool open(string path);
    bool open(DIData &data);
    
    bool read(DILong offset, DIData& data);
    bool write(DILong offset, DIData& data);
    
protected:
    DILong imageOffset;
    DILong imageSize;
    
    BlockRAWSectorOrder sectorOrder;
};
