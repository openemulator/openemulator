
/**
 * libdiskimage
 * DiskCopy 4.2 Block Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Decodes the DiskCopy 4.2 format
 */

#include "BlockData.h"

class BlockDC42 : public BlockData
{
public:
    BlockDC42();
    BlockDC42(string path);
    BlockDC42(DIData& data);
    ~BlockDC42();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool setProperty(string name, string value);
    bool getProperty(string name, string& value);
    
    bool read(DILong offset, DIData& data);
    bool write(DILong offset, DIData& data);
    
    bool readTag(DILong offset, DIData& data);
    bool writeTag(DILong offset, DIData& data);
    
private:
    DILong imageOffset;
    DILong imageSize;
    
    DILong tagOffset;
    DILong tagSize;
    
    DIInt gcrFormat;
    
    bool openDC42();
};
