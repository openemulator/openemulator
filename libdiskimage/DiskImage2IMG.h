
/**
 * libdiskimage
 * Disk Image 2IMG
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses 2IMG disk images
 */

#include "DiskImageFile.h"

class DiskImage2IMG
{
public:
    DiskImage2IMG();
    
    bool open(DiskImageFile *file);
    void close();
    
    bool isReadOnly();
    DILong getSize();
    string getSectorOrder();
    DIInt getGCRVolume();
    
    bool read(DILong pos, DIChar *buf, DILong num);
    bool write(DILong pos, const DIChar *buf, DILong num);
    
private:
    DiskImageFile *file;
    
    bool readOnly;
    string sectorOrder;
    DIInt gcrVolume;
    DILong imageOffset;
    DILong imageSize;
};
