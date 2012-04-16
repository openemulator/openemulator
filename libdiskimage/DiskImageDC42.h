
/**
 * libdiskimage
 * Disk Image DiskCopy 4.2
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses DiskCopy 4.2 disk images
 */

#include "DiskImageFile.h"

#define DI_DC42_TAGSIZE   12

class DiskImageDC42
{
public:
    DiskImageDC42();
    
    bool open(DiskImageFile *data);
    void close();
    
    DILong getSize();
    DIInt getGCRFormat();
    
    bool read(DILong pos, DIChar *buf, DILong num);
    bool write(DILong pos, const DIChar *buf, DILong num);
    
    bool readTag(DILong pos, DIChar *buf, DILong num);
    bool writeTag(DILong pos, const DIChar *buf, DILong num);
    
private:
    DiskImageFile *file;
    
    DIInt gcrFormat;
    
    DILong imageOffset;
    DILong imageSize;
    
    DILong tagOffset;
    DILong tagSize;
};
