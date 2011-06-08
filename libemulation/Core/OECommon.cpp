
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

int getInt(const string& value)
{
	if (value.substr(0, 2) == "0x")
	{
		unsigned int i;
		stringstream ss;
		ss << hex << value.substr(2);
		ss >> i;
		return i;
	}
	else
		return atoi(value.c_str());
}

double getFloat(const string& value)
{
	return atof(value.c_str());
}

string getString(int value)
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

string getHexString(int value)
{
	stringstream ss;
	ss << "0x" << hex << value;
	return ss.str();
}

OEData getCharVector(const string& value)
{
	OEData result;
	int start = (value.substr(0, 2) == "0x") ? 2 : 0;
	int size = (int) value.size() / 2 - start;
	result.resize(size);
	
	for (int i = 0; i < size; i++)
	{
		unsigned int n;
		stringstream ss;
		ss << hex << value.substr(start + i * 2, 2);
		ss >> n;
		result[i] = n;
	}
	
	return result;
}

int getNextPowerOf2(int value)
{
	return (int) pow(2, ceil(log2(value)));
}

bool readFile(string path, OEData *data)
{
	bool success = false;
	
	ifstream file(path.c_str());
	
	if (file.is_open())
	{
		file.seekg(0, ios::end);
		int size = (int) file.tellg();
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
	int extensionIndex = (int) path.rfind('.');
	if (extensionIndex == string::npos)
		return "";
	
	// Convert to lower case
	for (int i = 0; i < path.size(); i++)
		path[i] = tolower(path[i]);
	
	return path.substr(extensionIndex + 1);
}
