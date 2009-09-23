
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
	Package(string packagePath, bool write);
	~Package();

	bool isOpen();
	
	bool readFile(string localPath, vector<char> &data);
	bool writeFile(string localPath, vector<char> &data);
	
private:
	bool isPackageOpen;
	
	string packagePath;
	struct zip *zip;
};

#endif
