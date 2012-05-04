
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

static DIInt crcTable16[256];
static DIInt crcTable32[256];

static void buildCRCTables()
{
	for (DIInt i = 0; i < 256; i++)
    {
		DIShort w = i << 8;
		DIInt d = i;
        
		for (DIInt k = 0; k < 8; k++)
        {
			w = (w << 1) ^ ((w & 0x8000) ? 0x1021 : 0);
			d = (d >> 1) ^ (d & 1 ? 0xedb88320 : 0);
		}
        
		crcTable16[i] = w;
		crcTable32[i] = d;
	}
}

DIInt getDICRC16(DIChar *p, DIInt size)
{
    DIShort crc;
    
    if (!crcTable32[1])
        buildCRCTables();
    
    crc = 0xffff;
    
    while (size--)
        crc = (crc << 8) ^ crcTable16[((crc >> 8) ^ (*p++)) & 0xff];
    
    return crc;
}

DIInt getDICRC32(DIChar *p, DIInt size)
{
    DIInt crc;
    
    if (!crcTable32[1])
        buildCRCTables();
    
    crc = 0xffffffff;
    
    while (size--)
        crc = crcTable32[(crc ^ (*p++)) & 0xff] ^ (crc >> 8);
    
    return crc ^ 0xffffffff;
}
