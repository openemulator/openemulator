

/**
 * libemulation
 * Common types and functions
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common emulation types and functions
 */

#include <math.h>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "OECommon.h"

void logMessage(string message)
{
    cerr << "libemulation: " << message << endl;
}

OEInt getOEInt(const string& value)
{
    OEInt i;
    stringstream ss;
    
    if (value.substr(0, 2) == "0x")
        ss << hex << value.substr(2);
    else
        ss << value;
    ss >> i;
    
    return i;
}

OESInt getOESInt(const string& value)
{
    OESInt i;
    stringstream ss;
    
    if (value.substr(0, 2) == "0x")
        ss << hex << value.substr(2);
    else
        ss << value;
    ss >> i;
    
    return i;
}

OELong getOELong(const string& value)
{
    OELong i;
    stringstream ss;
    
    if (value.substr(0, 2) == "0x")
        ss << hex << value.substr(2);
    else
        ss << value;
    ss >> i;
    
    return i;
}

OESLong getOESLong(const string& value)
{
    OESLong i;
    stringstream ss;
    
    if (value.substr(0, 2) == "0x")
        ss << hex << value.substr(2);
    else
        ss << value;
    ss >> i;
    
    return i;
}

double getFloat(const string& value)
{
    double f;
    stringstream ss;
    
    ss << value;
    ss >> f;
    
    return f;
}

string getString(OEInt value)
{
    stringstream ss;
    
    ss << value;
    
    return ss.str();
}

string getString(OESInt value)
{
    stringstream ss;
    
    ss << value;
    
    return ss.str();
}

string getString(OELong value)
{
    stringstream ss;
    
    ss << value;
    
    return ss.str();
}

string getString(OESLong value)
{
    stringstream ss;
    
    ss << value;
    
    return ss.str();
}

string getString(float value)
{
    stringstream ss;
    
    ss << setprecision(10) << value;
    
    return ss.str();
}

string getHexString(OELong value)
{
    stringstream ss;
    
    ss << "0x" << hex << value;
    
    return ss.str();
}

string rtrim(string value)
{
    size_t found;
    
    found = value.find_last_not_of(" \n\r\t\f\v");
    
    if (found != string::npos)
        return value.substr(0, found + 1);
    
    return "";
}

wstring rtrim(wstring value)
{
    size_t found;
    
    found = value.find_last_not_of(L" \n\r\t\f\v");
    
    if (found != string::npos)
        return value.substr(0, found + 1);
    
    return L"";
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

string strfilter(string value, string filter)
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

OEData getCharVector(const string& value)
{
    OEData result;
    size_t start = (value.substr(0, 2) == "0x") ? 2 : 0;
    size_t size = (value.size() - start) / 2;
    
    result.resize(size);
    
    for (size_t i = 0; i < size; i++)
    {
        stringstream ss;
        OEInt n;
        ss << hex << value.substr(start + i * 2, 2);
        ss >> n;
        result[i] = n;
    }
    
    return result;
}

OELong getNextPowerOf2(OELong value)
{
    return (OELong) pow(2, ceil(log2(value)));
}

OEInt getBitNum(OELong value)
{
    OEInt bitNum = 0;
    for (OELong i = value; i > 1; i >>= 1)
        bitNum++;
    
    return bitNum;
}

bool readFile(string path, OEData *data)
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

bool writeFile(string path, OEData *data)
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

string getFilename(string path)
{
    size_t index;
    
    // Remove tailing path separator
    index = (path.length() - 1);
    if (path.find_last_of(OE_PATH_SEPARATOR) == index)
        path = path.substr(0, index);
    
    // Isolate filename
    index = path.find_last_of(OE_PATH_SEPARATOR);
    if (index != string::npos)
        path = path.substr(index + 1);
    
    return path;
}

string getPathExtension(string path)
{
    path = getFilename(path);
    
    // Find extension separator
    size_t index = path.find_last_of('.');
    if (index == string::npos)
        return "";
    path = path.substr(index + 1);
    
    // Convert to lower case
    for (size_t i = 0; i < path.size(); i++)
        path[i] = tolower(path[i]);
    
    return path;
}
