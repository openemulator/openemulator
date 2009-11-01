
/**
 * libemulator
 * Emulation Template
 * (C) 2009 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 */

#ifndef _OEPARSER_H
#define _OEPARSER_H

#include <string>
#include <vector>

#include <libxml/tree.h>

#include "OEDefines.h"

using namespace std;

typedef struct 
{
	string ref;
	string type;
	string options;
	string label;
} OESettingsInfo;

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
	
	vector<OESettingsInfo> settings;
} OEPortInfo;

typedef struct
{
	string label;
	string image;
	string description;
	string group;
	
	vector<OEPortInfo> inlets;
	vector<OEPortInfo> outlets;
} OEDMLInfo;

typedef struct
{
	string device;
	string component;
	string property;
} OESplitRef;

typedef vector<OEPortInfo> OEPortsInfo;

class OEParser
{
public:
	OEParser(string path);
	OEParser(xmlDocPtr dml);
	~OEParser();
	
	bool isOpen();
	
	OEDMLInfo *getDMLInfo();
	
private:
	bool open;
	
	OEDMLInfo dmlInfo;
	
	string getPathExtension(string path);
	bool readFile(string path, vector<char> &data);
	
	string getNodeProperty(xmlNodePtr node, string key);
	string buildAbsoluteRef(string absoluteRef, string ref);
	OESplitRef buildSplitRef(string ref);
	
	string getConnectedOutletRef(xmlDocPtr dml, string inletRef);
	OEPortInfo *getOutletProperties(string outletRef);
	string recurseAndBuildConnectedLabel(OEPortInfo *outlet, vector<string> *refList);
	string buildConnectedLabel(OEPortInfo *outlet);
	
	bool parse(xmlDocPtr dml);
	OEPortInfo parsePort(string deviceName,
						 string deviceLabel,
						 string deviceImage,
						 xmlNodePtr node);
};

#endif
