
/**
 * libemulation
 * OEInfo
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Parses a DML file.
 */

#ifndef _OEINFO_H
#define _OEINFO_H

#include <string>
#include <vector>

#include <libxml/tree.h>

#include "OERef.h"

using namespace std;

class OESetting
{
public:
	OESetting()
	{
	}
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
		
		connectionLabel = port.connectionLabel;
		connectionPort = port.connectionPort;
	}
	
	OERef ref;
	string type;
	string category;
	string label;
	string image;
	
	string deviceLabel;
	
	string connectionLabel;
	OEPort *connectionPort;
};

typedef vector<OEPort> OEPorts;

class OEInfo
{
public:
	OEInfo(string path);
	OEInfo(xmlDocPtr doc);
	OEInfo(const OEInfo &info);
	
	bool isLoaded();
	
	string getLabel();
	string getImage();
	string getDescription();
	string getGroup();
	OEPorts *getInlets();
	OEPorts *getOutlets();
	OEPorts *getSettings();
	
private:
	bool loaded;
	
	string label;
	string image;
	string description;
	string group;
	
	OEPorts inlets;
	OEPorts outlets;
	OESettings settings;
	
	string getPathExtension(string path);
	bool readFile(string path, vector<char> &data);
	string getXMLProperty(xmlNodePtr node, string name);
	
	xmlDocPtr loadDML(string path);
	void processDML(xmlDocPtr doc);
	
	void getDMLProperties(xmlDocPtr);
	bool validateDML(xmlDocPtr doc);
	void parseDML(xmlDocPtr doc);
	void parseDevice(xmlNodePtr node);
	OEPort parsePort(xmlNodePtr node, OERef ref, string label, string image);
	OESetting parseSetting(xmlNodePtr node, OERef ref);
	
	void setConnections(xmlDocPtr doc);
	xmlNodePtr getNodeForRef(xmlDocPtr doc, OERef ref);
	OERef getOutletForInlet(xmlDocPtr doc, OERef ref);
	OEPort *getPortForOutlet(OERef ref);
	
	void setConnectionLabels();
	string getConnectionLabel(OEPort *outletPort, vector<OERef> &visitedRefs);
};

#endif
