
/**
 * libemulation
 * Common types and functions
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Common OpenEmulator types and functions
 */

#include <math.h>
#include <sstream>
#include <fstream>

#include "OECommon.h"

void logMessage(string message)
{
	cerr << "libemulation: " << message << endl;
}

OEUInt32 getUInt32(const string& value)
{
    OEUInt32 i;
    stringstream ss;
    
	if (value.substr(0, 2) == "0x")
		ss << hex << value.substr(2);
	else
		ss << value;
    ss >> i;
    
    return i;
}

OEInt32 getInt32(const string& value)
{
    OEInt32 i;
    stringstream ss;
    
	if (value.substr(0, 2) == "0x")
		ss << hex << value.substr(2);
	else
		ss << value;
    ss >> i;
    
    return i;
}

OEUInt64 getUInt64(const string& value)
{
    OEUInt64 i;
    stringstream ss;
    
	if (value.substr(0, 2) == "0x")
		ss << hex << value.substr(2);
	else
		ss << value;
    ss >> i;
    
    return i;
}

OEInt64 getInt64(const string& value)
{
    OEInt64 i;
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
	return atof(value.c_str());
}

string getString(OEUInt32 value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string getString(OEInt32 value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string getString(OEUInt64 value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string getString(OEInt64 value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string getString(float value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

string getHexString(OEUInt64 value)
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

wstring getWString(string value)
{
    wstring ws;
    
	wchar_t w = 0;
	OEUInt32 bytes = 0;
    
	for (size_t i = 0; i < value.size(); i++)
    {
		unsigned char c = (unsigned char)value[i];
        
		if (c <= 0x7f)
        {
            // first byte
            bytes = 0;
			ws.push_back((wchar_t)c);
		}
		else if (c <= 0xbf)
        {
            // second/third/fourth byte
			if (bytes)
            {
				w = ((w << 6) | (c & 0x3f));
				bytes--;
				if (bytes == 0)
					ws.push_back(w);
			}
		}
		else if (c <= 0xdf)
        {
            // 2-byte sequence start
			bytes = 1;
			w = c & 0x1f;
		}
		else if (c <= 0xef)
        {
            // 3-byte sequence start
			bytes = 2;
			w = c & 0x0f;
		}
		else if (c <= 0xf7)
        {
            // 4-byte sequence start
			bytes = 3;
			w = c & 0x07;
		}
		else
			bytes = 0;
	}
    
    return ws;
}

OEData getCharVector(const string& value)
{
	OEData result;
	size_t start = (value.substr(0, 2) == "0x") ? 2 : 0;
	size_t size = value.size() / 2 - start;
	result.resize(size);
	
	for (size_t i = 0; i < size; i++)
	{
		OEUInt8 n;
		stringstream ss;
		ss << hex << value.substr(start + i * 2, 2);
		ss >> n;
		result[i] = n;
	}
	
	return result;
}

OEUInt64 getNextPowerOf2(OEUInt64 value)
{
	return (OEUInt64) pow(2, ceil(log2(value)));
}

bool readFile(string path, OEData *data)
{
	bool success = false;
	
	ifstream file(path.c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		streampos size = file.tellg();
		file.seekg(0, ios::beg);
		
		data->resize(size);
		file.read(&data->front(), size);
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
		file.write(&data->front(), data->size());
		success = file.good();
		file.close();
	}
	
	return success;
}

string getPathExtension(string path)
{
	// Remove tailing path separator
	if (path.rfind(OE_PATH_SEPARATOR) == (path.length() - 1))
		path = path.substr(0, path.length() - 1);
	
	// Find extension
	size_t extensionIndex = path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	
	// Convert to lower case
	for (size_t i = 0; i < path.size(); i++)
		path[i] = tolower(path[i]);
	
	return path.substr(extensionIndex + 1);
}
