
/**
 * libdiskimage
 * Block RAW Disk Image
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Accesses RAW disk images
 */

#include <fstream>

#include "DICommon.h"

class DiskImageData
{
public:
    DiskImageData();
    DiskImageData(string path);
    DiskImageData(DIData& data);
    ~DiskImageData();
    
    bool open(string path);
    bool open(DIData& data);
    bool is_open();
    void close();
    
    bool setProperty(string name, string value);
    bool getProperty(string name, string& value);
    
    bool read(DILong offset, DIChar *data, DILong size);
    bool write(DILong offset, DIChar *data, DILong size);
    
protected:
    DILong dataSize;
    
    bool readOnly;
    
private:
    fstream file;
    DIData data;
    
    void init();
};
