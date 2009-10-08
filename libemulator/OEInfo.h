
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
} OEDMLInfo;

typedef struct OEPortInfo
{
	string ref;
	string type;
	string subtype;
	string label;
	string image;
	
	string deviceLabel;
	
	string connectedLabel;
	struct OEPortInfo *connectedPort;
} OEPortInfo;

typedef struct
{
	string device;
	string component;
	string property;
} OESplitRef;

typedef vector<OEPortInfo> OEPortsInfo;

class OEInfo
{
public:
	OEInfo(string path);
	OEInfo(xmlDocPtr dml);
	~OEInfo();
	
	bool isOpen();
	
	OEDMLInfo *getDMLInfo();
	OEPortsInfo *getInletsInfo();
	OEPortsInfo *getOutletsInfo();
	
private:
	bool open;
	
	OEDMLInfo dmlInfo;
	OEPortsInfo inletsInfo;
	OEPortsInfo outletsInfo;
	
	string getPathExtension(string path);
	bool readFile(string path, vector<char> &data);
	
	string getNodeProperty(xmlNodePtr node, string key);
	string buildAbsoluteRef(string absoluteRef, string ref);
	OESplitRef buildSplitRef(string ref);
	
	string getConnectedOutletRef(xmlDocPtr dml, string inletRef);
	OEPortInfo *getOutletProperties(string outletRef);
	string buildConnectedLabel(OEPortInfo *outlet, vector<string> *refList);
	string buildConnectedLabel(OEPortInfo *outlet);
	
	bool parse(xmlDocPtr dml);
	OEPortInfo parsePort(string deviceName,
						 string deviceLabel,
						 string deviceImage,
						 xmlNodePtr node);
};

#endif
