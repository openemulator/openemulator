
/**
 * libemulation
 * Package class
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Type for accessing DML packages (zip and directory type)
 */

#include <fstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

#include "OEPackage.h"

OEPackage::OEPackage()
{
	init();
}

OEPackage::OEPackage(string path)
{
	init();
	open(path);
}

OEPackage::~OEPackage()
{
	close();
}

void OEPackage::init()
{
	is_open = false;
	
	zip = NULL;
}

bool OEPackage::open(string path)
{
	close();
	this->path = path;
	
	bool isPackage;
	if (isPathValid(path))
		isPackage = isFolder(path);
	else
		isPackage = (path.substr(path.size() - 1, 1) == 
					 OE_PATH_SEPARATOR);
	
	if (isPackage)
	{
		makeDirectory(path);
		is_open = isPathValid(path);
	}
	else
	{
		zip = zip_open(path.c_str(), ZIP_CREATE, NULL);
		is_open = (zip != NULL);
	}
	
	return is_open;
}

bool OEPackage::isOpen()
{
	return is_open;
}

void OEPackage::close()
{
	is_open = false;
	
	if (zip)
		zip_close(zip);
	zip = NULL;
}

bool OEPackage::readFile(string packagePath, OEData *data)
{
	bool error = true;
	
	if (!is_open)
		return false;
	
	if (zip)
	{
		struct zip_stat zipStat;
		struct zip_file *zipFile;
		
		if (zip_stat(zip, (const char *) packagePath.c_str(), 0, &zipStat) == 0)
		{
			if ((zipFile = zip_fopen(zip, packagePath.c_str(), 0)) != NULL)
			{
				data->resize(zipStat.size);
				error = (zip_fread(zipFile,
								   &data->front(), data->size()) !=
						 zipStat.size);
				zip_fclose(zipFile);
			}
		}
	}
	else
	{
		ifstream file((path + OE_PATH_SEPARATOR + packagePath).c_str());
		
		if (file.is_open())
		{
			file.seekg(0, ios::end);
			int size = file.tellg();
			file.seekg(0, ios::beg);
			
			data->resize(size);
			file.read(&data->front(), data->size());
			
			error = !file.good();
			
			file.close();
		}
	}
	
	return !error;
}

bool OEPackage::writeFile(string packagePath, OEData *data)
{
	bool error = true;
	
	if (!is_open)
		return false;
	
	if (zip)
	{
		struct zip_source *zipSource = NULL;
		
		if ((zipSource = zip_source_buffer(zip,
										   &data->front(), data->size(),
										   0)) != NULL)
		{
			int index;
			if ((index = zip_name_locate(zip, packagePath.c_str(), 0)) == -1)
				error = (zip_add(zip, packagePath.c_str(), zipSource) == -1);
			else
				error = (zip_replace(zip, index, zipSource) == -1);
			
			zip_source_free(zipSource);
		}
	}
	else
	{
		ofstream file((path + OE_PATH_SEPARATOR + packagePath).c_str());
		
		if (file.is_open())
		{
			file.write(&data->front(), data->size());
			error = !file.good();
			file.close();
		}
	}
	
	return !error;
}

bool OEPackage::remove()
{
	close();
	
	return removePath(path);
}

bool OEPackage::isPathValid(string path)
{
#ifdef _WIN32
	return (GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES);
#else
	struct stat statbuf;
	return (stat(path.c_str(), &statbuf) == 0);
#endif
}

bool OEPackage::isFolder(string path)
{
#ifdef _WIN32
	return (GetFileAttributes(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false;
	return (statbuf.st_mode & S_IFDIR);
#endif
}

bool OEPackage::makeDirectory(string path)
{
#ifdef _WIN32
	return CreateDirectory(path.c_str())
#else
	return (mkdir(path.c_str(), 0777) == 0);
#endif
}

bool OEPackage::removePath(string path)
{
#ifdef _WIN32
	if (!isFolder(path))
		return DeleteFile(path.c_str());
	
	string dirPath = path + OE_PATH_SEPARATOR + "*";
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(dirPath.c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	
	while (FindNextFile(hFind, &findFileData))
	{
		string fileName = findFileData.cFileName;
		if ((fileName == ".") || (fileName == ".."))
			continue;
		
		removePath(path + OE_PATH_SEPARATOR + fileName);
	}
	
	FindClose(hFind);
	
	return RemoveDirectory(path);
#else
	if (!isFolder(path))
		return (unlink(path.c_str()) == 0);
	
	DIR *dir;
	if (!(dir = opendir(path.c_str())))
		return false;
	
	struct dirent *dp;
	while ((dp = readdir(dir)) != NULL)
	{
		string fileName = string(dp->d_name);
		if ((fileName == ".") || (fileName == ".."))
			continue;
		
		removePath(path + OE_PATH_SEPARATOR + fileName);
	}
	
	closedir(dir);
	
	return rmdir(path.c_str());
#endif
}
