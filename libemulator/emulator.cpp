
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <dirent.h>
#include <iostream>

#include "libxml/encoding.h"

#include "emulator.h"

Emulation::Emulation()
{
}

Emulation::~Emulation()
{
}

bool Emulation::readDML(string path, DMLInfo &dmlInfo)
{
	return true;
}

bool Emulation::readTemplates(string templatesPath,
							  map<string, DMLInfo> &templates)
{
	DIR *dir;
	struct dirent *entry;
	
	if ((dir = opendir(templatesPath.c_str())) == NULL)
		return false;
		
	while ((entry = readdir(dir)) != NULL)
	{
		DMLInfo dmlInfo;
		string path = templatesPath + '/' + entry->d_name;
		
		if(path.substr(path.find_last_of(".") + 1) != "emulation")
			continue;
		
		if (entry->d_type | DT_DIR)
			path = path + "/info.xml";
		else
		{
			// Decompress info.xml from package
		}
		
		if (readDML(path, dmlInfo))
			templates[path] = dmlInfo;
		
		// Remove temporary file
	}

	closedir(dir);
	
	return true;
}

bool Emulation::readDMLs(string dmlsPath,
						 map<string, DMLInfo> &dmls)
{
	DIR *dir;
	struct dirent *entry;
	
	if ((dir = opendir(dmlsPath.c_str())) == NULL)
		return false;
	
	while ((entry = readdir(dir)) != NULL)
	{
		DMLInfo dmlInfo;
		string path = dmlsPath + '/' + entry->d_name;
		
		if(path.substr(path.find_last_of(".") + 1) != "xml")
			continue;
		
		if (readDML(path, dmlInfo))
			dmls[path] = dmlInfo;
	}
	
	closedir(dir);
	
	return true;
}

bool Emulation::open(string emulationPath)
{
	return true;
}

bool Emulation::runFrame()
{
	return true;
}

bool Emulation::save(string emulationPath)
{
	return true;
}

bool Emulation::ioctl(string componentName, int message, void * data)
{
	return 0;
}

bool Emulation::getOutlets(vector<DMLOutlet> &outlets)
{
	return true;
}

bool Emulation::getAvailableDMLs(map<string, DMLInfo> &dmls,
								 vector<string> &availableDMLs)
{
	return true;
}

bool Emulation::getAvailableInlets(DMLOutlet &outlet,
								   vector<DMLInlet> &availableInlets)
{
	return true;
}

bool Emulation::addDevice(string dmlPath,
						  map<string, string> outletInletMap)
{
	return true;
}

void Emulation::removeDevice(string deviceName)
{
}
