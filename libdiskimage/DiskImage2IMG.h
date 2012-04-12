
/**
 * libdiskimage
 * 2IMG Block Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses 2IMG disk images
 */

#include "BlockRAW.h"

class Block2IMG : public BlockRAW
{
public:
    Block2IMG();
    Block2IMG(string path);
    Block2IMG(DIData& data);
    ~Block2IMG();
    
    bool open(string path);
    bool open(DIData& data);
    void close();
    
    bool getProperty(string name, string& value);
    
    bool read(DILong offset, DIChar *data, DILong size);
    bool write(DILong offset, DIChar *data, DILong size);
    
private:
    DILong imageOffset;
    DILong imageSize;
    
    DIInt gcrVolume;
    
    bool open2IMG();
};
