
/**
 * libdiskimage
 * Common types and functions
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common emulation types and functions
 */

#ifndef _DICOMMON_H
#define _DICOMMON_H

#include <vector>
#include <string>

using namespace std;

typedef unsigned char DIChar;
typedef unsigned short DIShort;
typedef unsigned int DIInt;
typedef unsigned long long DILong;

typedef vector<DIChar> DIData;

#ifdef _WIN32
#define DI_PATH_SEPARATOR "\\"
#else
#define DI_PATH_SEPARATOR "/"
#endif

DIInt getDIInt(const string& value);
DILong getDILong(const string& value);
string getDIString(DIInt value);
string getDIString(DILong value);

DIShort getDIShortLE(DIChar *p);
DIShort getDIShortBE(DIChar *p);
DIInt getDIIntLE(DIChar *p);
DIInt getDIIntBE(DIChar *p);
DILong getDILongLE(DIChar *p);
DILong getDILongBE(DIChar *p);

string getDIPathExtension(string path);

#endif
