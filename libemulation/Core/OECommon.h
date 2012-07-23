
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

#include "util.h"

using namespace std;

#define OEAssertBit(x,m) ((x)|=(m))
#define OEClearBit(x,m) ((x)&=~(m))
#define OEToggleBit(x,m) ((x)^=(m))

#define OESetBit(x,m,v) if (v) OEAssertBit(x,m); else OEClearBit(x,m)
#define OEGetBit(x,m) (((x)&(m)) != 0)

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

void logMessage(string message);

OEInt getOEInt(const string& value);
OESInt getOESInt(const string& value);
OELong getOELong(const string& value);
OESLong getOESLong(const string& value);

float getFloat(const string& value);
double getDouble(const string& value);
OEData getCharVector(const string& value);

string getString(OEInt value);
string getString(OESInt value);
string getString(OELong value);
string getString(OESLong value);

#endif
