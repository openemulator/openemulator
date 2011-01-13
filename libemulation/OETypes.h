
/**
 * libemulation
 * Types
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
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

typedef struct
{
	float x;
	float y;
} OEPoint;

typedef struct
{
	float width;
	float height;
} OESize;

typedef struct
{
	OEPoint origin;
	OESize size;
} OERect;

#endif
