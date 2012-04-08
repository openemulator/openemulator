
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

typedef vector<unsigned char> DIData;

typedef unsigned short DIShort;
typedef unsigned int DIInt;
typedef unsigned long long DILong;

#ifdef _WIN32
#define DI_PATH_SEPARATOR "\\"
#else
#define DI_PATH_SEPARATOR "/"
#endif

DIInt getDIInt(const string& value);
string getDIString(DIInt value);
string getDIString(DILong value);

DIShort getDIShortLE(unsigned char *p);
DIShort getDIShortBE(unsigned char *p);
DIInt getDIIntLE(unsigned char *p);
DIInt getDIIntBE(unsigned char *p);

string getDIPathExtension(string path);

#endif
