

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

float getFloat(const string& value)
{
    float f;
    stringstream ss;
    
    ss << value;
    ss >> f;
    
    return f;
}

double getDouble(const string& value)
{
    double f;
    stringstream ss;
    
    ss << value;
    ss >> f;
    
    return f;
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
