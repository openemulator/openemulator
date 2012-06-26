
/**
 * libemulation
 * Common types and functions
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common emulation types and functions
 */

#ifndef _OECOMMON_H
#define _OECOMMON_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

#define OEAssertBit(x,m) ((x)|=(m))
#define OEClearBit(x,m) ((x)&=~(m))
#define OEToggleBit(x,m) ((x)^=(m))

#define OESetBit(x,m,v) if (v) OEAssertBit(x,m); else OEClearBit(x,m)
#define OEGetBit(x,m) ((x)&(m))

typedef unsigned char OEChar;
typedef signed char OESChar;
typedef unsigned short int OEShort;
typedef signed short int OESShort;
typedef unsigned int OEInt;
typedef signed int OESInt;
typedef unsigned long long OELong;
typedef signed long long OESLong;

typedef OELong OEAddress;

typedef union
{
#ifdef BYTES_BIG_ENDIAN
    struct { OEChar h7, h6, h5, h4, h3, h2, h, l; } b;
    struct { OESChar h7, h6, h5, h4, h3, h2, h, l; } sb;
    struct { OEShort h3, h2, h, l; } w;
    struct { OESShort h3, h2, h, l; } sw;
    struct { OEInt l, h; } d;
    struct { OESInt l, h; } sd;
#else
    struct { OEChar l, h, h2, h3, h4, h5, h6, h7; } b;
    struct { OESChar l, h, h2, h3, h4, h5, h6, h7; } sb;
    struct { OEShort l, h, h2, h3; } w;
    struct { OESShort l, h, h2, h3; } sw;
    struct { OEInt l, h; } d;
    struct { OESInt l, h; } sd;
#endif
    OELong q;
    OESLong qd;
} OEUnion;

typedef vector<OEChar> OEData;

#ifdef _WIN32
#define OE_PATH_SEPARATOR "\\"
#else
#define OE_PATH_SEPARATOR "/"
#endif

void logMessage(string message);

OEInt getOEInt(const string& value);
OESInt getOESInt(const string& value);
OELong getOELong(const string& value);
OESLong getOESLong(const string& value);

double getFloat(const string& value);
OEData getCharVector(const string& value);

string getString(OEInt value);
string getString(OESInt value);
string getString(OELong value);
string getString(OESLong value);

string getString(float value);
string getHexString(OELong value);

string rtrim(string value);
wstring rtrim(wstring value);
vector<string> strsplit(string value, char c);
string strjoin(vector<string>& value, char c);
string strfilter(string value, string filter);
string strtolower(string value);
string strtoupper(string value);

OELong getNextPowerOf2(OELong value);
OEInt getBitNum(OELong value);

bool readFile(string path, OEData *data);
bool writeFile(string path, OEData *data);

string getFilename(string path);
string getPathExtension(string path);

#endif
