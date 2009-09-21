
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

using namespace std;

class Package
{
public:
	Package(string path);
	~Package();

	bool isOpen();
	
	bool readFile(string packagePath, vector<char> &data);
	bool writeFile(string packagePath, vector<char> &data);
	
private:
	bool isPackageOpen;
	
	string path;
	struct zip *zip;
};

#endif
