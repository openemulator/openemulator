
/**
 * libutil
 * Common functions
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common functions
 */

#include <math.h>
#include <sstream>
#include <fstream>
#include <iomanip>

#ifdef __WIN32__
#include <windows.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

#include "util.h"

string getString(float value)
{
    stringstream ss;
    
    ss << setprecision(10) << value;
    
    return ss.str();
}

string getHexString(long long value)
{
    stringstream ss;
    
    ss << "0x" << hex << value;
    
    return ss.str();
}

string getBinString(long long value, int bitnum)
{
    string s;
    
    for (int mask = 1 << (bitnum - 1); mask; mask >>= 1)
        s += (value & mask) ? '1' : '0';
    
    return s;
}

string rtrim(string value)
{
    size_t found = value.find_last_not_of(" \n\r\t\f\v");
    
    if (found != string::npos)
        return value.substr(0, found + 1);
    
    return "";
}

wstring rtrim(wstring value)
{
    size_t found = value.find_last_not_of(L" \n\r\t\f\v");
    
    if (found != string::npos)
        return value.substr(0, found + 1);
    
    return L"";
}

string ltrim(string value)
{
    size_t found = value.find_first_not_of(" \n\r\t\f\v");
    
    if (found != string::npos)
        return value.substr(found);
    
    return "";
}

string trim(string value)
{
    return ltrim(rtrim(value));
}

vector<string> strsplit(string value, char c)
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

string strjoin(vector<string>& value, char c)
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

string strclean(string value, string filter)
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

string strsanitize(string value, string filter)
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

string strtolower(string value)
{
    string filteredString;
    
    for (string::iterator i = value.begin();
         i != value.end();
         i++)
        filteredString += tolower(*i);
    
    return filteredString;
}

string strtoupper(string value)
{
    string filteredString;
    
    for (string::iterator i = value.begin();
         i != value.end();
         i++)
        filteredString += toupper(*i);
    
    return filteredString;
}

long long getNextPowerOf2(long long value)
{
    return (long long) pow(2, ceil(log2(value)));
}

int getBitNum(long long value)
{
    int bitNum = 0;
    
    for (long long i = value; i > 1; i >>= 1)
        bitNum++;
    
    return bitNum;
}

string getLastPathComponent(string path)
{
    size_t index;
    
    // Remove tailing path separator
    index = (path.length() - 1);
    if (path.find_last_of(PATH_SEPARATOR) == index)
        path = path.substr(0, index);
    
    // Isolate filename
    index = path.find_last_of(PATH_SEPARATOR);
    if (index != string::npos)
        path = path.substr(index + 1);
    
    return path;
}

string getPathExtension(string path)
{
    path = getLastPathComponent(path);
    
    // Find extension separator
    size_t index = path.find_last_of('.');
    if (index == string::npos)
        return "";
    path = path.substr(index + 1);
    
    return path;
}

string getPathByDeletingPathExtension(string path)
{
    size_t length = getPathExtension(path).size();
    
    if (!length)
        return path;
    
    return path.substr(0, path.size() - length - 1);
}

string getPathByDeletingLastPathComponent(string path)
{
    size_t length = getLastPathComponent(path).size();
    
    if (!length)
        return path;
    
    return path.substr(0, path.size() - length - 1);
}

string getRandomFilename()
{
    string path;
    
    for (int i = 0; i < 16; i++)
    {
        char c = (random() & 0xff * 26 / 255);
        
        path += c;
    }
    
    return path;
}

bool readFile(string path, vector<unsigned char> *data)
{
    bool success = false;
    
    ifstream file(path.c_str());
    
    if (file.is_open())
    {
        file.seekg(0, ios::end);
        size_t size = (size_t) file.tellg();
        file.seekg(0, ios::beg);
        
        data->resize(size);
        
        file.read((char *) &data->front(), size);
        success = file.good();
        
        file.close();
    }
    
    return success;
}

bool writeFile(string path, vector<unsigned char> *data)
{
    bool success = false;
    
    ofstream file(path.c_str());
    
    if (file.is_open())
    {
        file.write((char *) &data->front(), data->size());
        success = file.good();
        
        file.close();
    }
    
    return success;
}

bool isPathValid(const string path)
{
#ifdef __WIN32__
    return (GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES);
#else
    struct stat statbuf;
    return (stat(path.c_str(), &statbuf) == 0);
#endif
}

bool isPathADirectory(const string path)
{
#ifdef __WIN32__
    return (GetFileAttributes(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    
    return (statbuf.st_mode & S_IFDIR);
#endif
}

bool createDirectory(const string path)
{
#ifdef __WIN32__
    return CreateDirectory(path.c_str())
#else
    return (mkdir(path.c_str(), 0777) == 0);
#endif
}

bool removePath(const string path)
{
#ifdef __WIN32__
    if (!isDirectory(path))
        return DeleteFile(path.c_str());
    
    string dirPath = path + OE_PATH_SEPARATOR + "*";
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(dirPath.c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE)
        return false;
    
    while (FindNextFile(hFind, &findFileData))
    {
        string filename = findFileData.cFileName;
        if ((filename == ".") || (filename == ".."))
            continue;
        
        removePath(path + OE_PATH_SEPARATOR + filename);
    }
    
    FindClose(hFind);
    
    return RemoveDirectory(path);
#else
    if (!isPathADirectory(path))
        return (unlink(path.c_str()) == 0);
    
    DIR *dir;
    if (!(dir = opendir(path.c_str())))
        return false;
    
    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL)
    {
        string filename = string(dp->d_name);
        if ((filename == ".") || (filename == ".."))
            continue;
        
        removePath(path + PATH_SEPARATOR + filename);
    }
    
    closedir(dir);
    
    return rmdir(path.c_str());
#endif
}

bool moveFile(string from, string to)
{
    return (rename(from.c_str(), to.c_str()) == 0);
}
