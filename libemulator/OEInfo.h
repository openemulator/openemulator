
/**
 * libemulator
 * Emulation Template
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _OEINFO_H
#define _OEINFO_H

#include <string>
#include <map>

#include <libxml/tree.h>

#include "OEComponent.h"
#include "Package.h"

using namespace std;

typedef map<string, string> OEProperties;
typedef map<string, OEProperties> OEPortProperties;

class OEInfo
{
public:
	OEInfo(string path);
	OEInfo(xmlDocPtr dml);
	~OEInfo();
	
	bool isOpen();
	
	OEProperties getProperties();
	OEPortProperties getInlets();
	OEPortProperties getOutlets();
	
private:
	bool open;
	
	OEProperties properties;
	OEPortProperties inlets;
	OEPortProperties outlets;
};

#endif
