
/**
 * libdiskimage
 * Apple 5.25" Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses an Apple 5.25" disk image
 */

#include "DiskImageData.h"
#include "DiskImage2IMG.h"
#include "DiskImageDC42.h"

#include "DiskImageApple525.h"

DiskImageApple525::DiskImageApple525()
{
    close();
}

DiskImageApple525::~DiskImageApple525()
{
    close();
}

bool DiskImageApple525::open(string path)
{
    DiskImageFile diskImageFile;
    
    if (!diskImageFile.open(path))
        return false;
    
    return open(&diskImageFile);
}

bool DiskImageApple525::open(DIData& data)
{
    DiskImageFile diskImageFile;
    
    if (!diskImageFile.open(data))
        return false;
    
    return open(&diskImageFile);
}

bool DiskImageApple525::open(DiskImageFile *diskImageFile)
{
    DiskImage2IMG diskImage2IMG;
    
    if (diskImage2IMG.open(diskImageFile))
    {
        // Read 35-40 tracks
        //   convert D13 to GCR53
        //   convert DSK to GCR62
        //   convert NIB to GCR
        
        return true;
    }
    
    DiskImageDC42 diskImageDC42;
    
    if (diskImageDC42.open(diskImageFile))
    {
        // Read 35-40 tracks
        //   convert D13 to GCR53
        //   convert DSK to GCR62
        //   convert NIB to GCR
        
        return true;
    }
    
    // Check file size, probe size for type of data
    // (35..40) * 13 * 256 for DOS 3.2
    // (35..40) * 16 * 256 for DOS 3.3/ProDOS/CPM (check extension)
    // (35..40) * 6384 for NIB
    // (35..40) * 6656 for NIB
    
    return true;
}

void DiskImageApple525::close()
{
}

bool DiskImageApple525::isReadOnly()
{
    return false;
}

DIDisk *DiskImageApple525::getDisk()
{
    return NULL;
}
