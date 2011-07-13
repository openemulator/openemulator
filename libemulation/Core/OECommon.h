
/**
 * libemulation
 * Common types and functions
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common OpenEmulator types and functions
 */

#ifndef _OECOMMON_H
#define _OECOMMON_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

#define OEGetBit(x,m) ((x)&(m))
#define OESetBit(x,m) ((x)|=(m))
#define OEClearBit(x,m) ((x)&=~(m))
#define OEToggleBit(x,m) ((x)^=(m))

typedef unsigned char OEUInt8;
typedef signed char OEInt8;
typedef unsigned short int OEUInt16;
typedef signed short int OEInt16;
typedef unsigned int OEUInt32;
typedef signed int OEInt32;
typedef unsigned long long OEUInt64;
typedef signed long long OEInt64;

typedef unsigned int OEAddress;

typedef union
{
#ifdef BYTES_BIG_ENDIAN
	struct { OEUInt8 l, h, h2, h3; } b;
	struct { OEInt8 l, h, h2, h3; } sb;
	struct { OEUInt16 l, h; } w;
	struct { OEInt16 l, h; } sw;
#else
	struct { OEUInt8 h3, h2, h, l; } b;
	struct { OEInt8 h3, h2, h, l; } sb;
	struct { OEUInt16 h, l; } w;
	struct { OEInt8 h, l; } sw;
#endif
	OEUInt32 d;
	OEInt32 sd;
} OEPair;

typedef vector<char> OEData;

#ifdef _WIN32
#define OE_PATH_SEPARATOR "\\"
#else
#define OE_PATH_SEPARATOR "/"
#endif

void logMessage(string message);

int getInt(const string& value);
double getFloat(const string& value);
OEData getCharVector(const string& value);

string getString(int value);
string getString(float value);
string getHexString(int value);
string rtrim(string value);

int getNextPowerOf2(int value);

bool readFile(string path, OEData *data);
bool writeFile(string path, OEData *data);
string getPathExtension(string path);

#endif
