
/**
 * libdiskimage
 * Common types and functions
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common types and functions
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

string getDIString(float value)
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

void setDIShortLE(DIChar *p, DIShort value)
{
    p[0] = (value >> 0);
    p[1] = (value >> 8);
}

void setDIShortBE(DIChar *p, DIShort value)
{
    p[0] = (value >> 8);
    p[1] = (value >> 0);
}

void setDIIntLE(DIChar *p, DIInt value)
{
    p[0] = (value >> 0);
    p[1] = (value >> 8);
    p[2] = (value >> 16);
    p[3] = (value >> 24);
}

void setDIIntBE(DIChar *p, DIInt value)
{
    p[0] = (value >> 24);
    p[1] = (value >> 16);
    p[2] = (value >> 8);
    p[3] = (value >> 0);
}

void setDILongLE(DIChar *p, DILong value)
{
    p[0] = (value >> 0);
    p[1] = (value >> 8);
    p[2] = (value >> 16);
    p[3] = (value >> 24);
    p[4] = (value >> 32);
    p[5] = (value >> 40);
    p[6] = (value >> 48);
    p[7] = (value >> 56);
}

void setDILongBE(DIChar *p, DILong value)
{
    p[0] = (value >> 56);
    p[1] = (value >> 48);
    p[2] = (value >> 40);
    p[3] = (value >> 32);
    p[4] = (value >> 24);
    p[5] = (value >> 16);
    p[6] = (value >> 8);
    p[7] = (value >> 0);
}
