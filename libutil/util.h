
/**
 * libutil
 * Common functions
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common functions
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

string getString(float value);
string getHexString(long long value);
string getBinString(long long value, int bitnum);

string rtrim(string value);
wstring rtrim(wstring value);
string ltrim(string value);
string trim(string value);
vector<string> strsplit(string value, char c);
string strjoin(vector<string>& value, char c);
string strclean(string value, string filter);
string strsanitize(string value, string filter);
string strtolower(string value);
string strtoupper(string value);

long long getNextPowerOf2(long long value);
int getBitNum(long long value);

string getLastPathComponent(string path);
string getPathExtension(string path);
string getPathByDeletingPathExtension(string path);
string getPathByDeletingLastPathComponent(string path);
string getRandomFilename();

bool readFile(string path, vector<unsigned char> *data);
bool writeFile(string path, vector<unsigned char> *data);

bool isPathValid(const string path);
bool isPathADirectory(const string path);
bool createDirectory(const string path);
bool removePath(const string path);
bool moveFile(string from, string to);

#endif
