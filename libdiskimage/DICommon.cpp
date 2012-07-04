
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

string getDIFilename(string path)
{
    size_t index;
    
    // Remove tailing path separator
    index = (path.length() - 1);
    if (path.find_last_of(DI_PATH_SEPARATOR) == index)
        path = path.substr(0, index);
    
    // Isolate filename
    index = path.find_last_of(DI_PATH_SEPARATOR);
    if (index != string::npos)
        path = path.substr(index + 1);
    
    return path;
}

string getDIPathExtension(string path)
{
    path = getDIFilename(path);
    
    // Find extension separator
    size_t index = path.find_last_of('.');
    if (index == string::npos)
        return "";
    path = path.substr(index + 1);
    
    // Convert to lower case
    for (size_t i = 0; i < path.size(); i++)
        path[i] = tolower(path[i]);
    
    return path;
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

vector<string> strDISplit(string value, char c)
{
    vector<string> splitString;
    
    size_t startPos = 0;
    size_t endPos = value.find(c, startPos);
    
    while (endPos != string::npos)
    {
        splitString.push_back(value.substr(startPos, endPos - startPos));
        
        startPos = endPos + 1;
        endPos = value.find(c, startPos);
    }
    
    splitString.push_back(value.substr(startPos));
    
    return splitString;
}

string strDIJoin(vector<string>& value, char c)
{
    string joinedString;
    
    for (vector<string>::iterator i = value.begin();
         i != value.end();
         i++)
    {
        if (joinedString != "")
            joinedString += c;
        
        joinedString += *i;
    }
    
    return joinedString;
}

string rtrimDI(string value)
{
    size_t found = value.find_last_not_of(" \n\r\t\f\v");
    
    if (found != string::npos)
        return value.substr(0, found + 1);
    
    return "";
}

string ltrimDI(string value)
{
    size_t found = value.find_first_not_of(" \n\r\t\f\v");
    
    if (found != string::npos)
        return value.substr(found);
    
    return "";
}

string trimDI(string value)
{
    return ltrimDI(rtrimDI(value));
}

string strDIIncludeFilter(string value, string filter)
{
    string filteredString;
    
    for (string::iterator i = value.begin();
         i != value.end();
         i++)
    {
        if (filter.find(*i) != string::npos)
            filteredString += *i;
    }
    
    return filteredString;
}

string strDIExcludeFilter(string value, string filter)
{
    string filteredString;
    
    for (string::iterator i = value.begin();
         i != value.end();
         i++)
    {
        if (filter.find(*i) == string::npos)
            filteredString += *i;
    }
    
    return filteredString;
}
