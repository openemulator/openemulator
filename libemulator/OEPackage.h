
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

#define PACKAGE_PATH_SEPARATOR "/"

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
};

#endif
