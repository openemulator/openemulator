
/**
 * libemulator
 * Package class
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _PACKAGE_H
#define _PACKAGE_H

#include <string>
#include <vector>

#include <zip.h>

#define PACKAGE_PATH_SEPARATOR "/"

using namespace std;

class Package
{
public:
	Package(string packagePath);
	~Package();

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
