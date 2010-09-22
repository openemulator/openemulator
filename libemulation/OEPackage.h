
/**
 * libemulation
 * Package class
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Type for accessing DML packages (zip and directory type)
 */

#ifndef _OEPACKAGE_H
#define _OEPACKAGE_H

#include <zip.h>

#include "OETypes.h"

#ifdef _WIN32
#define OE_PATH_SEPARATOR "\\"
#else
#define OE_PATH_SEPARATOR "/"
#endif

using namespace std;

class OEPackage
{
public:
	OEPackage();
	OEPackage(string path);
	~OEPackage();
	
	bool open(string path);
	bool isOpen();
	void close();
	
	bool readFile(string packagePath, OEData *data);
	bool writeFile(string packagePath, OEData *data);
	
	bool remove();
	
private:
	bool is_open;
	
	string path;
	struct zip *zip;
	
	void init();
	
	bool isPathValid(string path);
	bool isFolder(string path);
	bool makeDirectory(string path);
	bool removePath(string path);
};

#endif
