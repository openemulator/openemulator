
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

#include <libxml/tree.h>

#include "OEDefines.h"

using namespace std;

typedef struct
{
	string label;
	string image;
	string description;
	string group;
} OEProperties;

typedef struct
{
	string ref;
	string type;
	string subtype;
	string label;
	string image;
	bool connected;
} OEPortProperties;

typedef vector<OEPortProperties> OEPorts;

class OEInfo
{
public:
	OEInfo(string path);
	OEInfo(xmlDocPtr dml);
	~OEInfo();
	
	bool isOpen();
	
	OEProperties *getProperties();
	OEPorts *getInlets();
	OEPorts *getOutlets();
	
private:
	bool open;
	
	OEProperties properties;
	OEPorts inlets;
	OEPorts outlets;
	
	string getPathExtension(string path);
	bool readFile(string path, vector<char> &data);
	
	string getNodeProperty(xmlNodePtr node, string key);
	string buildAbsoluteRef(string absoluteRef, string ref);
	
	string getConnectionRef(xmlDocPtr dml,
							string inletRef);
	OEPortProperties *getOutlet(string outletRef);
	OEPortProperties *getOutletFromInlet(xmlDocPtr dml, string inletRef);
	
	bool parse(xmlDocPtr dml);
	OEPortProperties parsePort(string deviceName, xmlNodePtr node);
};

#endif
