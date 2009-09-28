
/**
 * libemulator
 * Emulation Template
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _OEINFO_H
#define _OEINFO_H

#include <string>
#include <vector>
#include <map>

#include <libxml/tree.h>

#include "OEDefines.h"

using namespace std;

typedef map<string, string> OEProperties;
typedef map<string, OEProperties> OEPorts;

class OEInfo
{
public:
	OEInfo(string path);
	OEInfo(xmlDocPtr dml);
	~OEInfo();
	
	bool isOpen();
	
	OEProperties getProperties();
	OEPorts getInlets();
	OEPorts getOutlets();
	
private:
	bool open;
	
	OEProperties properties;
	OEPorts inlets;
	OEPorts outlets;
	
	string getPathExtension(string path);
	bool readFile(string path, vector<char> &data);
	
	string getNodeProperty(xmlNodePtr node, string key);
	string buildAbsoluteRef(string absoluteRef, string ref);
	
	string getConnection(xmlDocPtr dml, string connectionRef);
	
	bool parse(xmlDocPtr dml);
	void parsePort(OEPorts &ports, string deviceName, xmlNodePtr node);
};

#endif
