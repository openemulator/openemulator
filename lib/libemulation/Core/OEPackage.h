
/**
 * libemulation
 * OEPackage
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a file package (zip and directory type)
 */

#ifndef _OEPACKAGE_H
#define _OEPACKAGE_H

#include <zip.h>

#include "OECommon.h"

using namespace std;

class OEPackage
{
public:
    OEPackage();
    ~OEPackage();
    
    bool open(const string& path);
    bool isOpen();
    void close();
    
    bool read(const string& packagePath, OEData *data);
    bool write(const string& packagePath, OEData *data);
    
    bool remove();
    
private:
    bool is_open;
    
    string path;
    struct zip *zip;
};

#endif
