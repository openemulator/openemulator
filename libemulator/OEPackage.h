
/**
 * libemulator
 * Package class
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Type for accessing DML packages (zip and directory type)
 */

#ifndef _OEPACKAGE_H
#define _OEPACKAGE_H

#include <string>
#include <vector>

#include <zip.h>

#ifdef _WIN32
#define OE_PATH_SEPARATOR "\\"
#else
#define OE_PATH_SEPARATOR "/"
#endif

#define OE_EXTENSION "emulation"
#define OE_INFO_FILENAME "info.xml"

using namespace std;

class OEPackage
{
public:
	OEPackage(string packagePath);
	~OEPackage();

	bool isOpen();
	
	bool readFile(string localPath, vector<char> &data);
	bool writeFile(string localPath, vector<char> &data);
	
	bool remove();
	
private:
	bool open;
	
	string packagePath;
	struct zip *zip;
	
	bool isPathValid(string path);
	bool isFolder(string path);
	bool makeDirectory(string path);
	bool removePath(string path);
};

#endif
