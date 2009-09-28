
/**
 * libemulator
 * Package class
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#include <fstream>
#include <sys/stat.h>

#include "Package.h"

Package::Package(string packagePath)
{
	this->packagePath = packagePath;
	zip = NULL;
	
	struct stat statbuf;
	bool isPathCreated = (stat(packagePath.c_str(), &statbuf) == 0);
	
	bool isPackage;
	if (isPathCreated)
		isPackage = (statbuf.st_mode & S_IFDIR);
	else
		isPackage = (packagePath.substr(packagePath.size() - 1, 1) == "/");
	
	if (isPackage)
	{
		mkdir(packagePath.c_str(), 0777);
		open = (stat(packagePath.c_str(), &statbuf) == 0);
	}
	else
	{
		zip = zip_open(packagePath.c_str(), ZIP_CREATE, NULL);
		open = (zip != NULL);
	}
}

Package::~Package()
{
	if (zip)
		zip_close(zip);
	
	zip = NULL;
}

bool Package::isOpen()
{
	return open;
}

bool Package::readFile(string localPath, vector<char> &data)
{
	bool error = true;
	
	if (!open)
		return false;
	
	if (zip)
	{
		struct zip_stat zipStat;
		struct zip_file *zipFile;
		
		if (zip_stat(zip, (const char *) localPath.c_str(), 0, &zipStat) == 0)
		{
			if ((zipFile = zip_fopen(zip, localPath.c_str(), 0)) != NULL)
			{
				data.resize(zipStat.size);
				error = (zip_fread(zipFile, &data[0], zipStat.size) != zipStat.size);
				zip_fclose(zipFile);
			}
		}
	}
	else
	{
		ifstream file((packagePath + PACKAGE_PATH_SEPARATOR + localPath).c_str());
		
		if (file.is_open())
		{
			file.seekg(0, ios::end);
			int size = file.tellg();
			file.seekg(0, ios::beg);
			
			data.resize(size);
			file.read(&data[0], size);
			
			error = !file.good();
			
			file.close();
		}
	}
	
	return !error;
}

bool Package::writeFile(string localPath, vector<char> &data)
{
	bool error = true;
	
	if (!open)
		return false;
	
	if (zip)
	{
		struct zip_source *zipSource = NULL;
		
		if ((zipSource = zip_source_buffer(zip, &data[0], data.size(), 0)) != NULL)
		{
			int index;
			if ((index = zip_name_locate(zip, localPath.c_str(), 0)) == -1)
				error = (zip_add(zip, localPath.c_str(), zipSource) == -1);
			else
				error = (zip_replace(zip, index, zipSource) == -1);
			
			zip_source_free(zipSource);
		}
	}
	else
	{
		ofstream file((packagePath + PACKAGE_PATH_SEPARATOR + localPath).c_str());
		
		if (file.is_open())
		{
			file.write(&data[0], data.size());
			error = !file.good();
			file.close();
		}
	}
	
	return !error;
}

bool Package::remove()
{
	// To-Do: Remove

	return true;
}