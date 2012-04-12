
/**
 * libdiskimage
 * Common types and functions
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common emulation types and functions
 */

#include <sstream>

#include "DICommon.h"

DIInt getDIInt(const string& value)
{
    DIInt i;
    stringstream ss;
    
    ss << value;
    ss >> i;
    
    return i;
}

DILong getDILong(const string& value)
{
    DILong i;
    stringstream ss;
    
    ss << value;
    ss >> i;
    
    return i;
}

string getDIString(DIInt value)
{
    stringstream ss;
    
    ss << value;
    
    return ss.str();
}

string getDIString(DILong value)
{
    stringstream ss;
    
    ss << value;
    
    return ss.str();
}

DIShort getDIShortLE(DIChar *p)
{
    return ((p[0] << 0) |
            (p[1] << 8));
}

DIShort getDIShortBE(DIChar *p)
{
    return ((p[0] << 8) |
            (p[1] << 0));
}

DIInt getDIIntLE(DIChar *p)
{
    return ((p[0] << 0) |
            (p[1] << 8) |
            (p[2] << 16) |
            (p[3] << 24));
}

DIInt getDIIntBE(DIChar *p)
{
    return ((p[0] << 24) |
            (p[1] << 16) |
            (p[2] << 8) |
            (p[3] << 0));
}

DILong getDILongLE(DIChar *p)
{
    return (((DILong) p[0] << 0) |
            ((DILong) p[1] << 8) |
            ((DILong) p[2] << 16) |
            ((DILong) p[3] << 24) |
            ((DILong) p[4] << 32) |
            ((DILong) p[5] << 40) |
            ((DILong) p[6] << 48) |
            ((DILong) p[7] << 56));
}

DILong getDILongBE(DIChar *p)
{
    return (((DILong) p[0] << 56) |
            ((DILong) p[1] << 48) |
            ((DILong) p[2] << 40) |
            ((DILong) p[3] << 32) |
            ((DILong) p[4] << 24) |
            ((DILong) p[5] << 16) |
            ((DILong) p[6] << 8) |
            ((DILong) p[7] << 0));
}
string getDIPathExtension(string path)
{
    // Remove tailing path separator
    if (path.rfind(DI_PATH_SEPARATOR) == (path.length() - 1))
        path = path.substr(0, path.length() - 1);
    
    // Find extension
    size_t extensionIndex = path.rfind('.');
    if (extensionIndex == string::npos)
        return "";
    
    // Convert to lower case
    for (size_t i = 0; i < path.size(); i++)
        path[i] = tolower(path[i]);
    
    return path.substr(extensionIndex + 1);
}
