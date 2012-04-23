
/**
 * libdiskimage
 * Common types and functions
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common types and functions
 */

#ifndef _DICOMMON_H
#define _DICOMMON_H

#define DI_VERSION "1.0"

#include <vector>
#include <string>

using namespace std;

typedef unsigned char DIChar;
typedef unsigned short DIShort;
typedef unsigned int DIInt;
typedef unsigned long long DILong;

typedef signed char DISChar;
typedef signed short DISShort;
typedef signed int DISInt;
typedef signed long long DISLong;

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
string getDIString(float value);

DIShort getDIShortLE(DIChar *p);
DIShort getDIShortBE(DIChar *p);
DIInt getDIIntLE(DIChar *p);
DIInt getDIIntBE(DIChar *p);
DILong getDILongLE(DIChar *p);
DILong getDILongBE(DIChar *p);

void setDIShortLE(DIChar *p, DIShort value);
void setDIShortBE(DIChar *p, DIShort value);
void setDIIntLE(DIChar *p, DIInt value);
void setDIIntBE(DIChar *p, DIInt value);
void setDILongLE(DIChar *p, DILong value);
void setDILongBE(DIChar *p, DILong value);

string getDIPathExtension(string path);

#endif
