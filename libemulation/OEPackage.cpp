
/**
 * libemulation
 * Package class
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Type for accessing DML packages (zip and directory type)
 */

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

#include <fstream>

#include "OEPackage.h"

OEPackage::OEPackage(string packagePath)
{
	this->packagePath = packagePath;
	zip = NULL;
	
	if (!packagePath.size())
		return;
	
	bool isPackage;
	if (isPathValid(packagePath))
		isPackage = isFolder(packagePath);
	else
		isPackage = (packagePath.substr(packagePath.size() - 1, 1) == 
					 OE_PATH_SEPARATOR);
	
	if (isPackage)
	{
		makeDirectory(packagePath);
		open = isPathValid(packagePath);
	}
	else
	{
		zip = zip_open(packagePath.c_str(), ZIP_CREATE, NULL);
		open = (zip != NULL);
	}
}

OEPackage::~OEPackage()
{
	if (zip)
		zip_close(zip);
	
	zip = NULL;
}

bool OEPackage::isOpen()
{
	return open;
}

bool OEPackage::readFile(string localPath, vector<char> &data)
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
				error = (zip_fread(zipFile, &data[0], zipStat.size) !=
						 zipStat.size);
				zip_fclose(zipFile);
			}
		}
	}
	else
	{
		ifstream file((packagePath + OE_PATH_SEPARATOR + localPath).c_str());
		
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

bool OEPackage::writeFile(string localPath, vector<char> &data)
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
		ofstream file((packagePath + OE_PATH_SEPARATOR + localPath).c_str());
		
		if (file.is_open())
		{
			file.write(&data[0], data.size());
			error = !file.good();
			file.close();
		}
	}
	
	return !error;
}

bool OEPackage::remove()
{
	if (zip)
		zip_close(zip);
	
	zip = NULL;
	
	return removePath(packagePath);
}

bool OEPackage::isPathValid(string path)
{
#ifdef _WIN32
	return (GetFileAttributes(path.c_str()) != INVALID_FILE_ATTRIBUTES);
#else
	struct stat statbuf;
	return (stat(packagePath.c_str(), &statbuf) == 0);
#endif
}

bool OEPackage::isFolder(string path)
{
#ifdef _WIN32
	return (GetFileAttributes(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat statbuf;
	if (stat(packagePath.c_str(), &statbuf) != 0)
		return false;
	return (statbuf.st_mode & S_IFDIR);
#endif
}

bool OEPackage::makeDirectory(string path)
{
#ifdef _WIN32
	return CreateDirectory(path.c_str())
#else
	return (mkdir(packagePath.c_str(), 0777) == 0);
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
