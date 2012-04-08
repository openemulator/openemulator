
/**
 * libdiskimage
 * Block data disk image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a data type for handling disk image data
 */

#include <fstream>

#include "BlockDiskImage.h"

class BlockData : public BlockDiskImage
{
public:
    BlockData();
    BlockData(string path);
    BlockData(DIData& data);
    ~BlockData();
    
    bool open(string path);
    bool open(DIData& data);
    bool is_open();
    void close();
    
    bool setProperty(string name, string value);
    bool getProperty(string name, string& value);
    
    bool read(DILong offset, DIData& data);
    bool write(DILong offset, DIData& data);
    
protected:
    DILong dataSize;
    
    bool readOnly;
    
private:
    fstream file;
    DIData data;
    
    void init();
};
