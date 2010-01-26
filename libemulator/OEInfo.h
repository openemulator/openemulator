
/**
 * libemulator
 * OEInfo
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Parses a DML file
 */

#ifndef _OEINFO_H
#define _OEINFO_H

#include <string>
#include <vector>

#include <libxml/tree.h>

#include "OEDefines.h"
#include "OERef.h"

using namespace std;

class OESetting
{
public:
	OESetting(const OESetting &setting)
	{
		ref = setting.ref;
		type = setting.type;
		options = setting.options;
		label = setting.label;
	}
	
	OERef ref;
	string type;
	string options;
	string label;
};

typedef vector<OESetting> OESettings;

class OEPort
{
public:
	OEPort()
	{
	}
	OEPort(const OEPort &port)
	{
		ref = port.ref;
		type = port.type;
		category = port.category;
		label = port.label;
		image = port.image;
		
		deviceLabel = port.deviceLabel;
		
		connectedLabel = port.connectedLabel;
		connectedPort = port.connectedPort;
		
		settings = port.settings;
	}
	
	OERef ref;
	string type;
	string category;
	string label;
	string image;
	
	string deviceLabel;
	
	string connectedLabel;
	OEPort *connectedPort;
	
	OESettings settings;
};

typedef vector<OEPort> OEPorts;

class OEInfo
{
public:
	OEInfo(string path);
	OEInfo(xmlDocPtr dml);
	OEInfo(const OEInfo &info);
	
	bool isLoaded();
	
	string getLabel();
	string getImage();
	string getDescription();
	string getGroup();
	OEPorts *getInlets();
	OEPorts *getOutlets();
	
private:
	bool loaded;
	
	string label;
	string image;
	string description;
	string group;
	
	OEPorts inlets;
	OEPorts outlets;
	
	string getPathExtension(string path);
	bool readFile(string path, vector<char> &data);
	string getXMLProperty(xmlNodePtr node, string name);
	
	void parse(xmlDocPtr doc);
	bool validateDML(xmlDocPtr doc);
	void parseDML(xmlDocPtr doc);
	void parseDevice(xmlNodePtr node);
	OEPort parsePort(xmlNodePtr node, OERef ref, string label, string image);
	
	void setConnections(xmlDocPtr doc);
	xmlNodePtr getNodeForRef(xmlDocPtr doc, OERef ref);
	OERef getOutletRefForInletRef(xmlDocPtr doc, OERef ref);
	OEPort *getOutletPortForOutletRef(OERef ref);
	
	void setLabels();
	string setLabel(OEPort *outlet, vector<OERef> *visitedRefs);
};

#endif
