
/**
 * libdiskimage
 * Block RAW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses RAW disk images
 */

#include "BlockDiskImage.h"
#include "DiskImageData.h"

class BlockRAW : public BlockDiskImage
{
public:
    BlockRAW();
    BlockRAW(string path);
    BlockRAW(DIData& data);
    
    bool open(string path);
    bool open(DIData& data);
    bool is_open();
    void close();
    
    bool setProperty(string name, string value);
    bool getProperty(string name, string& value);
    
    bool read(DILong blockIndex, DIChar *blockData, DIInt blockNum);
    bool write(DILong blockIndex, DIChar *blockData, DIInt blockNum);
    
private:
    DiskImageData diskImage;
};
