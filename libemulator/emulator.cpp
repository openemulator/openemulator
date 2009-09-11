
/**
 * libemulator
 * Emulator interface
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <stdio.h>
#include <dirent.h>
#include <iostream>

#include "zip.h"
#include "libxml/encoding.h"

#include "emulator.h"

#define DMLINFO_FILENAME "info.xml"

Emulation::Emulation()
{
}

Emulation::~Emulation()
{
}

bool Emulation::readDML(char *dmlData, int dmlDataSize, DMLInfo &dmlInfo)
{
	return false;
}

bool Emulation::readDML(string path, DMLInfo &dmlInfo)
{
	bool isError = true;

	FILE *dmlFile;
	if ((dmlFile = fopen(path.c_str(), "rb")) != NULL)
	{
		fseek(dmlFile, 0, SEEK_END);
		int dmlFileSize = ftell(dmlFile);
		fseek(dmlFile, 0, SEEK_SET);
		
		char dmlData[dmlFileSize];
		if (fread(dmlData, 1, dmlFileSize, dmlFile) == dmlFileSize)
			isError = readDML(dmlData, dmlFileSize, dmlInfo);
		
		fclose(dmlFile);
	}
	
	return !isError;
}

bool Emulation::readTemplate(string path, DMLInfo &dmlInfo)
{
	bool isError = true;
	
	struct zip *zipFile;
	if ((zipFile = zip_open(path.c_str(), 0, NULL)) != NULL)
	{
		struct zip_stat dmlFileStat;
		if (zip_stat(zipFile, DMLINFO_FILENAME, 0, &dmlFileStat) == 0)
		{
			int dmlFileSize = dmlFileStat.size;
			struct zip_file *dmlFile;
			
			if ((dmlFile = zip_fopen(zipFile, DMLINFO_FILENAME, 0)) != NULL)
			{
				char dmlData[dmlFileSize];
				if (zip_fread(dmlFile, dmlData, dmlFileSize) == dmlFileSize)
					isError = readDML(dmlData, dmlFileSize, dmlInfo);

				zip_fclose(dmlFile);
			}
		}
		
		zip_close(zipFile);
	}
	
	return !isError;
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

bool Emulation::getInlets(vector<DMLInlet> &inlets)
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

bool Emulation::addDML(string dmlPath,
					   map<string, string> outletInletMap)
{
	return true;
}

void Emulation::removeDevicesOnOutlet(string outletName)
{
}
