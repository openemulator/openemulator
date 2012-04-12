
/**
 * libdiskimage
 * DiskCopy 4.2 Block Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses DiskCopy 4.2 disk images
 */

#include "BlockRAW.h"

class BlockDC42 : public BlockRAW
{
public:
    BlockDC42();
    BlockDC42(string path);
    BlockDC42(DIData& data);
    ~BlockDC42();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool getProperty(string name, string& value);
    
    bool read(DILong offset, DIChar *data, DILong size);
    bool write(DILong offset, DIChar *data, DILong size);
    
    bool readTag(DILong offset, DIChar *data, DILong size);
    bool writeTag(DILong offset, DIChar *data, DILong size);
    
private:
    DILong imageOffset;
    DILong imageSize;
    
    DILong tagOffset;
    DILong tagSize;
    
    DIInt gcrFormat;
    
    bool openDC42();
};
