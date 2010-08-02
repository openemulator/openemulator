
/**
 * libemulation
 * Types
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * General OpenEmulator types
 */

#ifndef _OETYPES_H
#define _OETYPES_H

#include <string>
#include <vector>
#include <map>

#include <iostream>

using namespace std;

inline void OELog(string text)
{
	cerr << "libemulator: " << text << endl;
}

#define OEGetBit(x,m) ((x)&(m))
#define OESetBit(x,m) ((x)|=(m))
#define OEClearBit(x,m) ((x)&=~(m))
#define OEToggleBit(x,m) ((x)^=(m))

typedef unsigned char OEUInt8;
typedef char OEInt8;
typedef unsigned short OEUInt16;
typedef short OEInt16;
typedef unsigned int OEUInt32;
typedef int OEInt32;

typedef union
{
#ifndef OE_LSB_FIRST
	struct { OEUInt8 h3, h2, h, l; } b;
	struct { OEInt8 h3, h2, h, l; } sb;
	struct { OEUInt16 h, l; } w;
	struct { OEInt8 h, l; } sw;
#else
	struct { OEUInt8 l, h, h2, h3; } b;
	struct { OEInt8 l, h, h2, h3; } sb;
	struct { OEUInt16 l, h; } w;
	struct { OEInt16 l, h; } sw;
#endif
	OEUInt32 d;
	OEInt32 sd;
} OEPair;

typedef vector<char> OEData;

#endif
